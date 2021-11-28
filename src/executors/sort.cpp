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
    parsedQuery->sortColumnName = tokenizedQuery[3];
    parsedQuery->sortRelationName = tokenizedQuery[5];
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
    result->blockCount = table->blockCount;
    result->columnCount = table->columnCount;
    result->columns = table->columns;
    result->maxRowsPerBlock = table->maxRowsPerBlock;
    result->rowCount = table->rowCount;
    result->rowsPerBlockCount.resize(table->blockCount * 3, 0);
    tableCatalogue->insertTable(result);

    Cursor cursor = table->getCursor();
    vector<int> row = cursor.getNext();

    // First pass

}