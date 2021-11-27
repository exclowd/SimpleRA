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
    std::for_each(rColumns.begin(), rColumns.end(), [&](string &column) {
        column = R->tableName + "." + column;
    });
    auto sColumns = S->columns;
    std::for_each(sColumns.begin(), sColumns.end(), [&](string &column) {
        column = S->tableName + "." + column;
    });
    auto resultantColumns = rColumns;
    std::move(sColumns.begin(), sColumns.end(), back_inserter(resultantColumns));
    auto *resultantTable = new Table(parsedQuery->joinResultRelationName, resultantColumns);
    int col1 = R->getColumnIndex(parsedQuery->joinFirstColumnName);
    int col2 = S->getColumnIndex(parsedQuery->joinSecondColumnName);

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
                auto *page = new Page(R->tableName, start + i);
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
                Page *pageS = new Page(S->tableName, page);
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
                            BufferManager::writePage(resultantTable->tableName, resultantTable->blockCount, rows,
                                                     (int) rows.size());
                            resultantTable->blockCount++;
                            rows.clear();
                        }
                    }
                }
            }
            if (!rows.empty()) {
                resultantTable->rowsPerBlockCount.emplace_back(rows.size());
                BufferManager::writePage(resultantTable->tableName, resultantTable->blockCount, rows,
                                         (int) rows.size());
                resultantTable->blockCount++;
                rows.clear();
            }
        }
    } else if (parsedQuery->joinStrategy == PARTHASH) {
        // pass
    }

    if (resultantTable->rowCount)
        tableCatalogue->insertTable(resultantTable);
    else {
        cout << "Empty Table" << endl;
        resultantTable->unload();
        delete resultantTable;
    }
}