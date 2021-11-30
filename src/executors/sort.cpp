#include <algorithm>
#include"../global.h"

/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT() {
    logger->log("syntacticParseSORT");

    if ((tokenizedQuery.size() != 8 && tokenizedQuery.size() != 10) || tokenizedQuery[4] != "BY" ||
        tokenizedQuery[6] != "IN") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    if (tokenizedQuery.size() == 10 && tokenizedQuery[8] != "BUFFER") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery->queryType = SORT;
    parsedQuery->sortResultRelationName = tokenizedQuery[0];
    parsedQuery->sortRelationName = tokenizedQuery[3];
    parsedQuery->sortColumnName = tokenizedQuery[5];
    parsedQuery->sortBufferSize = tokenizedQuery.size() == 10 ? stoi(tokenizedQuery[9]) : 10;
    string sortingStrategy = tokenizedQuery[7];
    if (sortingStrategy == "ASC")
        parsedQuery->sortingStrategy = ASC;
    else if (sortingStrategy == "DESC")
        parsedQuery->sortingStrategy = DESC;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseSORT() {
    logger->log("semanticParseSORT");

    if (tableCatalogue->isTable(parsedQuery->sortResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue->isTable(parsedQuery->sortRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue->isColumnFromTable(parsedQuery->sortColumnName, parsedQuery->sortRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

void executeSORT() {
    logger->log("executeSORT");

    Table *table = tableCatalogue->getTable(parsedQuery->sortRelationName);
    auto *result = new Table(parsedQuery->sortResultRelationName, table->columns);
    int columnIndex = table->getColumnIndex(parsedQuery->sortColumnName);
    tableCatalogue->insertTable(result);

    Cursor cursor = table->getCursor();

    int M = parsedQuery->sortBufferSize;
    // number of rows that can be loaded into the buffer
    int ROWS = M * (int) table->maxRowsPerBlock;
    int CHUNKS = ((int) table->rowCount + ROWS - 1) / ROWS;
    vector<int> pagesInChunk(CHUNKS);
    auto getSortTableName = [](const string &tableName, int bufferNo) {
        return tableName + "_sort_" + to_string(bufferNo);
    };

    auto getMergeTableName = [](const string &tableName, int passNo, int bufferNo) {
        return tableName + "_merge_" + to_string(passNo) + "_" + to_string(bufferNo);
    };

    vector<Table *> chunks(CHUNKS);

    // first pass: created sorted buffer each M blocks in size
    for (int it = 0; it * ROWS < table->rowCount; it++) {
        chunks[it] = new Table(getSortTableName(result->tableName, it), table->columns);
        tableCatalogue->insertTable(chunks[it]);
        int end = min((int) table->rowCount - it * ROWS, ROWS);
        vector<vector<int>> rows;
        rows.reserve(ROWS);
        for (int i = 0; i < end; i++) {
            vector<int> row = cursor.getNext();
            rows.push_back(std::move(row));
        }
        // after buffer has been filled with rows sort the buffer
        if (!rows.empty()) {
            sort(rows.begin(), rows.end(), [&](const vector<int> &I, const vector<int> &J) {
                if (parsedQuery->sortingStrategy == ASC)
                    return I[columnIndex] < J[columnIndex];
                return I[columnIndex] > J[columnIndex];
            });
        }
        // Now it is time to write the sorted buffer to disk
        for (int i = 0; i < rows.size(); i += (int) table->maxRowsPerBlock) {
            int num = (int) min(rows.size() - i, table->maxRowsPerBlock);
            vector<vector<int>> temp{rows.begin() + i, rows.begin() + i + num};
            chunks[it]->rowsPerBlockCount.emplace_back(temp.size());
            BufferManager::writePage(chunks[it]->tableName, chunks[it]->blockCount, temp, (int) temp.size());
            chunks[it]->blockCount++;
        };
    }
    // there are CHUNKS number of chunks and each chunk has M pages in it.

    // Now merge the runs
    int N = M - 1;
    int passes = (int) ceil(log(CHUNKS) / log(N));
    int K = CHUNKS;
    for (int pass = 0; pass < passes; pass++) { // logn passes
        // load the chunks in batches on N into memory and merge
        vector<Table *> tempChunks;
        for (int it = 0; it * N < K; it++) {
            tempChunks.push_back(new Table(getMergeTableName(result->tableName, pass, it), table->columns));
            tableCatalogue->insertTable(tempChunks[it]);
            vector<vector<int>> rows;
            int num = min(N, K - N * it);
            vector<Cursor> cursors;
            cursors.reserve(num);
            // get the cursors
            vector<vector<int>> mergeVector(num);
            for (int i = 0; i < num; i++) {
                cursors.push_back(chunks[it * N + i]->getCursor());
                mergeVector[i] = cursors[i].getNext();
            }
            // now we are merging these tables;
            int done = 0;
            while (done < num) {
                vector<vector<int>> temp;
                vector<Cursor> tempCursor;
                int req_index = min_element(mergeVector.begin(), mergeVector.end(),
                                            [&](const vector<int> &I, const vector<int> &J) {
                                                if (parsedQuery->sortingStrategy == ASC)
                                                    return I[columnIndex] < J[columnIndex];
                                                return I[columnIndex] > J[columnIndex];
                                            }) - mergeVector.begin();
                rows.push_back(mergeVector[req_index]);
                mergeVector[req_index] = cursors[req_index].getNext();
                for (int i = 0; i < mergeVector.size(); i++) {
                    if (mergeVector[i].empty()) {
                        done++;
                    } else {
                        temp.push_back(move(mergeVector[i]));
                        tempCursor.push_back(move(cursors[i]));
                    }
                }
                for (int i = 1; i < temp.size(); i++) {
                    assert(temp[i].size() == temp[i - 1].size());
                }
                if (rows.size() == table->maxRowsPerBlock) {
                    tempChunks[it]->rowsPerBlockCount.emplace_back(rows.size());
                    BufferManager::writePage(tempChunks[it]->tableName, tempChunks[it]->blockCount, rows,
                                             (int) rows.size());
                    tempChunks[it]->blockCount++;
                    rows.clear();
                }
                mergeVector = move(temp);
                cursors = move(tempCursor);
            }

            if (!rows.empty()) {
                tempChunks[it]->rowsPerBlockCount.emplace_back(rows.size());
                BufferManager::writePage(tempChunks[it]->tableName, tempChunks[it]->blockCount, rows,
                                         (int) rows.size());
                tempChunks[it]->blockCount++;
            }
        }
        for (auto &x: chunks) {
            tableCatalogue->deleteTable(x->tableName);
        };
        chunks.clear();
        K = (int) tempChunks.size();
        chunks = move(tempChunks);
    }
    assert(chunks.size() == 1);
    Cursor fcursor = chunks[0]->getCursor();
    vector<int> row = fcursor.getNext();
    vector<vector<int>> rows;
    while (!row.empty()) {
        result->updateStatistics(row);
        rows.push_back(row);
        if (rows.size() == result->maxRowsPerBlock) {
            result->rowsPerBlockCount.emplace_back(rows.size());
            BufferManager::writePage(result->tableName, result->blockCount, rows, (int) rows.size());
            result->blockCount++;
            rows.clear();
        }
        row = fcursor.getNext();
    }
    if (!rows.empty()) {
        result->rowsPerBlockCount.emplace_back(rows.size());
        BufferManager::writePage(result->tableName, result->blockCount, rows, (int) rows.size());
        result->blockCount++;
    }
    tableCatalogue->deleteTable(chunks[0]->tableName);
    if (result->rowCount)
        tableCatalogue->insertTable(result);
    else {
        cout << "Empty Table" << endl;
        result->unload();
        delete result;
    }
}