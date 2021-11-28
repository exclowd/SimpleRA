#include <regex>
#include "../global.h"

/**
 * @brief 
 * SYNTAX: R <- JOIN USING method relation_name1, relation_name2 ON column_name1 bin_op column_name2 BUFFER buffer
 */
bool syntacticParseJOIN() {
    logger->log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[3] != "USING" || tokenizedQuery[7] != "ON" ||
        tokenizedQuery[11] != "BUFFER") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery->queryType = JOIN;
    parsedQuery->joinResultRelationName = tokenizedQuery[0];

    string strategy = tokenizedQuery[4];
    if (strategy == "NESTED") {
        parsedQuery->joinStrategy = NESTED;
    } else if (strategy == "PARTHASH") {
        parsedQuery->joinStrategy = PARTHASH;
    } else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery->joinFirstRelationName = tokenizedQuery[5];
    parsedQuery->joinSecondRelationName = tokenizedQuery[6];
    parsedQuery->joinFirstColumnName = tokenizedQuery[8];
    parsedQuery->joinSecondColumnName = tokenizedQuery[10];
    regex isNumeric("[0-9]+");
    if (regex_match(tokenizedQuery[12], isNumeric)) {
        parsedQuery->joinBufferSize = stoi(tokenizedQuery[12]);
    } else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "==")
        parsedQuery->joinBinaryOperator = EQUAL;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN() {
    logger->log("semanticParseJOIN");

    if (tableCatalogue->isTable(parsedQuery->joinResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue->isTable(parsedQuery->joinFirstRelationName) ||
        !tableCatalogue->isTable(parsedQuery->joinSecondRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue->isColumnFromTable(parsedQuery->joinFirstColumnName, parsedQuery->joinFirstRelationName) ||
        !tableCatalogue->isColumnFromTable(parsedQuery->joinSecondColumnName, parsedQuery->joinSecondRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeJOIN() {
    logger->log("executeJOIN");
    Table *R = tableCatalogue->getTable(parsedQuery->joinFirstRelationName);
    Table *S = tableCatalogue->getTable(parsedQuery->joinSecondRelationName);
    if (R->blockCount > S->blockCount) {
        swap(R, S);
    }
    auto rColumns = R->columns;
    string &firstName = R->tableName;
    std::for_each(rColumns.begin(), rColumns.end(), [&](string &column) {
        column = firstName + "." + column;
    });
    auto sColumns = S->columns;
    string &secondName = S->tableName;
    std::for_each(sColumns.begin(), sColumns.end(), [&](string &column) {
        column = secondName + "." + column;
    });
    auto resultantColumns = rColumns;
    std::move(sColumns.begin(), sColumns.end(), back_inserter(resultantColumns));
    auto *resultantTable = new Table(parsedQuery->joinResultRelationName, resultantColumns);
    int col1 = R->getColumnIndex(parsedQuery->joinFirstColumnName);
    int col2 = S->getColumnIndex(parsedQuery->joinSecondColumnName);

    string &resultantName = resultantTable->tableName;
    if (parsedQuery->joinStrategy == NESTED) {
        int M = parsedQuery->joinBufferSize - 2;
        // outer loop
        for (int start = 0; start < R->blockCount; start += M) {

            // here we load all the pages into a hash_table
            unordered_map<int, vector<pair<int, int>>> hash_table;
            // load buffer with pages
            int end = min(M, (int) R->blockCount - start);

            vector<Page *> buffer;
            buffer.reserve(M);

            for (int i = 0; i < end; i++) {
                auto *page = new Page(firstName, start + i);
                buffer.push_back(page);
                for (int j = 0; j < page->rowCount; j++) {
                    // jth row in i-th buffer
                    // buffer[i]->rows[j]
                    hash_table[page->rows[j][col1]].emplace_back(i, j);
                }
            }
            vector<vector<int>> rows;
            // here is the nested loop
            for (int page = 0; page < S->blockCount; page++) {
                Page *pageS = new Page(secondName, page);
                for (const auto &row: pageS->rows) {

                    for (auto[i, j]: hash_table[row[col2]]) {
                        vector<int> to_add = buffer[i]->rows[j];
                        // make the resultant row
                        std::copy(row.begin(), row.end(), std::back_inserter(to_add));
                        // add row to table
                        resultantTable->updateStatistics(to_add);
                        rows.push_back(to_add);

                        if (rows.size() == resultantTable->maxRowsPerBlock) {
                            resultantTable->rowsPerBlockCount.emplace_back(rows.size());
                            BufferManager::writePage(resultantName, resultantTable->blockCount, rows,
                                                     (int) rows.size());
                            resultantTable->blockCount++;
                            rows.clear();
                        }
                    }
                }
                delete pageS;
            }
            if (!rows.empty()) {
                resultantTable->rowsPerBlockCount.emplace_back(rows.size());
                BufferManager::writePage(resultantName, resultantTable->blockCount, rows,
                                         (int) rows.size());
                resultantTable->blockCount++;
                rows.clear();
            }
            buffer.clear();
        }
    } else if (parsedQuery->joinStrategy == PARTHASH) {
        int B = parsedQuery->joinBufferSize - 1;

        partitionCatalogue = new PartitionCatalogue(resultantName, B, R, S);
        vector<HashPage *> buffer;
        buffer.assign(B, nullptr);

        vector<int> cnt(B); // how many pages does each partition have

        for (int i = 0; i < R->blockCount; i++) { // go over each of the blocks of R.
            Page *pg = new Page(firstName, i); // load page of table into memory
            for (const auto &row: pg->rows) {
                auto hash_function = hash<int>();
                int partition = (int) hash_function(row[col1]) % B; // hash based on the column value
                if (buffer[partition] == nullptr) {
                    buffer[partition] = new HashPage(resultantName, firstName, partition,
                                                     cnt[partition]++, R->columnCount);
                }
                buffer[partition]->addRow(row);
                if (buffer[partition]->rows.size() == R->maxRowsPerBlock) { // if page is full in memory dump to disk
                    partitionCatalogue->writePage(partition, firstName, buffer[partition]);
                    buffer[partition] = nullptr;
                }
            }
            delete pg;
        }

        for (int i = 0; i < B; i++) {
            if (buffer[i] != nullptr) { // dump all remaining pages in memory to disk
                partitionCatalogue->writePage(i, firstName, buffer[i]);
                buffer[i] = nullptr;
            }
        }

        std::fill(cnt.begin(), cnt.end(), 0);

        for (int i = 0; i < S->blockCount; i++) { // go over each of the blocks of S.
            Page *pg = new Page(secondName, i); // load page of table into memory
            for (const auto &row: pg->rows) {
                auto hash_function = hash<int>();
                int partition = (int) hash_function(row[col2]) % B; // hash based on the column value
                if (buffer[partition] == nullptr) {
                    buffer[partition] = new HashPage(resultantName, secondName, partition,
                                                     cnt[partition]++, S->columnCount);
                }
                buffer[partition]->addRow(row);
                if (buffer[partition]->rows.size() == S->maxRowsPerBlock) { // if page is full in memory dump to disk
                    partitionCatalogue->writePage(partition, secondName, buffer[partition]);
                    buffer[partition] = nullptr;
                }
            }
            delete pg;
        }
        for (int i = 0; i < B; i++) {
            if (buffer[i] != nullptr) { // dump all remaining pages to disk
                partitionCatalogue->writePage(i, secondName, buffer[i]);
                buffer[i] = nullptr;
            }
        }

        // now both of the tables have been partitioned and saved onto disk
        // rows to push into new table;
        vector<vector<int>> rows;

        for (int partition = 0; partition < B; partition++) { // go over each partition
            // load R partition into memory and into hashmap
            int pgIndex = 0, line = 0;
            // get the partition containing all the pages of R in partition
            Partition *pR = partitionCatalogue->getPartition(partition, firstName);
            Partition *pS = partitionCatalogue->getPartition(partition, secondName);
            while (pgIndex < pR->blockCount) {
                unordered_map<int, vector<vector<int>>> hash_table;
                size_t MAX_HASHMAP_SIZE = (B - 1) * R->maxRowsPerBlock;
                while (pgIndex < pR->blockCount) { // try to load as many pages into hashmap
                    auto *page = new HashPage(resultantName, partition, firstName, pgIndex);
                    int suc = false;
                    int i;
                    for (i = line; i < page->rowCount; i++) {
                        if (hash_table.size() == MAX_HASHMAP_SIZE) {
                            suc = true;
                            break;
                        } else {
                            hash_table[page->rows[i][col1]].push_back(page->rows[i]);
                        }
                    }
                    if (suc) {
                        line = i;
                        delete page;
                        break;
                    }
                    line = 0;
                    pgIndex++;
                    delete page;
                }
                cout << "loaded into hashmap" << endl;
                // Now iterate over the  pages of S in partition
                for (int i = 0; i < pS->blockCount; i++) {
//                    cout << i << endl;
                    auto *pageS = new HashPage(resultantName,  partition, secondName, i);
                    for (const auto &row: pageS->rows) {
//                        cout << "hi" << endl;
                        for (const auto &x: hash_table[row[col2]]) {
                            vector<int> to_add = x;
                            // make the resultant row
                            std::copy(row.begin(), row.end(), std::back_inserter(to_add));
                            // add row to table
                            resultantTable->updateStatistics(to_add);
                            rows.push_back(to_add);
                            if (rows.size() == resultantTable->maxRowsPerBlock) {
                                resultantTable->rowsPerBlockCount.emplace_back(rows.size());
                                BufferManager::writePage(resultantName, resultantTable->blockCount, rows,
                                                         (int) rows.size());
                                resultantTable->blockCount++;
                                rows.clear();
                            }
                        }
                    }
                    delete pageS;
                }
            }
        }

        if (!rows.empty()) {
            resultantTable->rowsPerBlockCount.emplace_back(rows.size());
            BufferManager::writePage(resultantName, resultantTable->blockCount, rows,
                                     (int) rows.size());
            resultantTable->blockCount++;
            rows.clear();
        }
    }


    if (resultantTable->rowCount)
        tableCatalogue->insertTable(resultantTable);
    else {
        cout << "Empty Table" << endl;
        resultantTable->unload();
        delete resultantTable;
    }
}