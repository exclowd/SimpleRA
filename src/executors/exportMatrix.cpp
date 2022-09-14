#include "../global.h"

/**
 * @brief 
 * SYNTAX: EXPORTMATRIX <relation_name> 
 */

bool syntacticParseEXPORTMATRIX() {
    logger->log("syntacticParseEXPORTMATRIX");
    if (tokenizedQuery.size() != 3 || tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery->queryType = EXPORTMATRIX;
    parsedQuery->exportMatrixName = tokenizedQuery[2];
    return true;
}

bool semanticParseEXPORTMATRIX() {
    logger->log("semanticParseEXPORTMATRIX");
    //Matrix should exist
    if (matrixCatalogue->isMatrix(parsedQuery->exportMatrixName))
        return true;
    cout << "SEMANTIC ERROR: No such matrix exists" << endl;
    return false;
}

void executeEXPORTMATRIX() {
    logger->log("executeEXPORTMATRIX");
    Matrix *matrix = matrixCatalogue->getMatrix(parsedQuery->exportMatrixName);
    matrix->makePermanent();
}