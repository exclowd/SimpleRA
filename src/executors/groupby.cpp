#include <map>

#include "../global.h"

/**
 * @brief
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> RETURN MAX|MIN|SUM|AVG(<attribute>)
 */
bool syntacticParseGROUPBY() {
    logger->log("syntacticParseGROUPBY");

    if (tokenizedQuery.size() != 9 || tokenizedQuery[3] != "BY" || tokenizedQuery[5] != "FROM" ||
        tokenizedQuery[7] != "RETURN" || tokenizedQuery[8].length() <= 5 ||
        tokenizedQuery[8][tokenizedQuery[8].length() - 1] != ')' || tokenizedQuery[8][3] != '(' ||
        (tokenizedQuery[8].substr(0, 3) != "MAX" && tokenizedQuery[8].substr(0, 3) != "MIN" &&
         tokenizedQuery[8].substr(0, 3) != "SUM" && tokenizedQuery[8].substr(0, 3) != "AVG")) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery->queryType = GROUPBY;
    parsedQuery->groupByResultRelationName = tokenizedQuery[0];
    parsedQuery->groupByGroupingAttributeName = tokenizedQuery[4];
    parsedQuery->groupByRelationName = tokenizedQuery[6];
    parsedQuery->groupByOperatorName = tokenizedQuery[8].substr(0, 3);
    parsedQuery->groupByAttributeName = tokenizedQuery[8].substr(4, tokenizedQuery[8].length() - 5);

    return true;
}

bool semanticParseGROUPBY() {
    logger->log("semanticParseGROUPBY");

    if (tableCatalogue->isTable(parsedQuery->groupByResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }
    if (!tableCatalogue->isTable(parsedQuery->groupByRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue->isColumnFromTable(parsedQuery->groupByGroupingAttributeName, parsedQuery->groupByRelationName) ||
        !tableCatalogue->isColumnFromTable(parsedQuery->groupByAttributeName, parsedQuery->groupByRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

void executeGROUPBY() {
    logger->log("executeGROUPBY");
    Table *table = tableCatalogue->getTable(parsedQuery->groupByRelationName);
    auto *resultantTable = new Table(parsedQuery->groupByResultRelationName, {parsedQuery->groupByGroupingAttributeName,
                                                                              parsedQuery->groupByOperatorName +
                                                                                  parsedQuery->groupByAttributeName});

    int i = table->getColumnIndex(parsedQuery->groupByGroupingAttributeName);
    int j = table->getColumnIndex(parsedQuery->groupByAttributeName);

    map<int, int> result;
    map<int, int> count;

    Cursor cursor = table->getCursor();
    vector<int> row = cursor.getNext();

    while (!row.empty()) {
        if (parsedQuery->groupByOperatorName == "MAX") {
            if (result.count(row[i]) == 0) {
                result[row[i]] = row[j];
            }
            result[row[i]] = max(result[row[i]], row[j]);
        } else if (parsedQuery->groupByOperatorName == "MIN") {
            if (result.count(row[i]) == 0) {
                result[row[i]] = row[j];
            }
            result[row[i]] = max(result[row[i]], row[j]);
        } else {
            result[row[i]] += row[j];
            if (parsedQuery->groupByOperatorName == "AVG") {
                count[row[i]]++;
            }
        }
        row = cursor.getNext();
    }
    //
    vector<vector<int>> rows;
    for (const auto &[x, y] : result) {
        if (parsedQuery->groupByOperatorName == "AVG") {
            resultantTable->updateStatistics({x, y / count[x]});
            rows.push_back({x, y / count[x]});
        } else {
            resultantTable->updateStatistics({x, y});
            rows.push_back({x, y});
        }

        if (rows.size() == resultantTable->maxRowsPerBlock) {
            resultantTable->rowsPerBlockCount.emplace_back(rows.size());
            BufferManager::writePage(resultantTable->tableName, resultantTable->blockCount, rows, (int)rows.size());
            resultantTable->blockCount++;
            rows.clear();
        }
    }
    //
    if (!rows.empty()) {
        resultantTable->rowsPerBlockCount.emplace_back(rows.size());
        BufferManager::writePage(resultantTable->tableName, resultantTable->blockCount, rows, (int)rows.size());
        resultantTable->blockCount++;
        rows.clear();
    }
    cout << resultantTable->rowCount << endl;
    if (resultantTable->rowCount)
        tableCatalogue->insertTable(resultantTable);
    else {
        cout << "Empty Table" << endl;
        resultantTable->unload();
        delete resultantTable;
    }
}