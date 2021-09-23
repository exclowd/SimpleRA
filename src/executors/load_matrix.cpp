#include "../global.h"

/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOADMATRIX() {
    logger.log("syntacticParseLOADMATRIX");
    if (tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOADMATRIX;
    parsedQuery.loadMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseLOADMATRIX() {
    logger.log("semanticParseLOADMATRIX");
    if (matrixCatalogue.isMatrix(parsedQuery.loadMatrixName)) {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadMatrixName)) {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOADMATRIX() {
    logger.log("executeLOADMATRIX");

    auto *matrix = new Matrix(parsedQuery.loadMatrixName);
    if (matrix->load()) {
        matrixCatalogue.insertMatrix(matrix);
        cout << "Loaded Table. Column Count: " << matrix->columnCount << " Row Count: " << matrix->rowCount << endl;
    }
}