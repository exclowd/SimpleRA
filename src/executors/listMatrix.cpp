#include "../global.h"

/**
 * @brief 
 * SYNTAX: LIST TABLES
 */
bool syntacticParseLISTMATRIX() {
    logger.log("syntacticParseLIST");
    if (tokenizedQuery.size() != 2 || tokenizedQuery[1] != "MATRIX") {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LISTMATRIX;
    return true;
}

bool semanticParseLISTMATRIX() {
    logger.log("semanticParseLISTMATRIX");
    return true;
}

void executeLISTMATRIX() {
    logger.log("executeLISTMATRIX");
    matrixCatalogue.print();
}