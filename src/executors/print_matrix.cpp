#include "../global.h"

/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINTMATRIX() {
    logger.log("syntacticParsePRINTMATRIX");
    if (tokenizedQuery.size() != 3) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;
    parsedQuery.printRelationName = tokenizedQuery[1];
    return true;
}

bool semanticParsePRINTMATRIX() {
    logger.log("semanticParsePRINTMATRIX");
    if (!MatrixCatalogue.isMatrix(parsedQuery.printRelationName)) {
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }
    return true;
}

void executePRINTMATRIX() {
    logger.log("executePRINTMATRIX");
    auto matrix = MatrixCatalogue.getMatrix(parsedQuery.printRelationName);
    matrix->print();
}
