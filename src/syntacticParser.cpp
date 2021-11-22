#include "global.h"
#include <sys/stat.h>

bool syntacticParse() {
    logger.log("syntacticParse");
    string possibleQueryType = tokenizedQuery[0];

    if (tokenizedQuery.size() < 2) {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    if (possibleQueryType == "CLEAR")
        return syntacticParseCLEAR();
    else if (possibleQueryType == "INDEX")
        return syntacticParseINDEX();
    else if (possibleQueryType == "LIST")
        return tokenizedQuery[1] == "MATRIX" ? syntacticParseLISTMATRIX() : syntacticParseLIST();
    else if (possibleQueryType == "LOAD")
        return tokenizedQuery[1] == "MATRIX" ? syntacticParseLOADMATRIX() : syntacticParseLOAD();
    else if (possibleQueryType == "PRINT")
        return tokenizedQuery[1] == "MATRIX" ? syntacticParsePRINTMATRIX() : syntacticParsePRINT();
    else if (possibleQueryType == "RENAME")
        return syntacticParseRENAME();
    else if (possibleQueryType == "EXPORT")
        return tokenizedQuery[1] == "MATRIX" ? syntacticParseEXPORTMATRIX() : syntacticParseEXPORT();
    else if (possibleQueryType == "SOURCE")
        return syntacticParseSOURCE();
    else if (possibleQueryType == "TRANSPOSE")
        return syntacticParseTRANSPOSE();
    else {
        string resultantRelationName = possibleQueryType;
        if (tokenizedQuery[1] != "<-" || tokenizedQuery.size() < 3) {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        possibleQueryType = tokenizedQuery[2];
        if (possibleQueryType == "PROJECT")
            return syntacticParsePROJECTION();
        else if (possibleQueryType == "SELECT")
            return syntacticParseSELECTION();
        else if (possibleQueryType == "JOIN")
            return syntacticParseJOIN();
        else if (possibleQueryType == "CROSS")
            return syntacticParseCROSS();
        else if (possibleQueryType == "DISTINCT")
            return syntacticParseDISTINCT();
        else if (possibleQueryType == "SORT")
            return syntacticParseSORT();
        else if (possibleQueryType == "GROUP")
            return syntacticParseGROUPBY();
        else {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
    }
    return false;
}

ParsedQuery::ParsedQuery() = default;

void ParsedQuery::clear() {
    logger.log("ParseQuery::clear");
    this->queryType = UNDETERMINED;

    this->clearRelationName = "";

    this->crossResultRelationName = "";
    this->crossFirstRelationName = "";
    this->crossSecondRelationName = "";

    this->distinctResultRelationName = "";
    this->distinctRelationName = "";

    this->exportRelationName = "";
    this->exportMatrixName = "";


    this->indexingStrategy = IndexingStrategy::NOTHING;
    this->indexColumnName = "";
    this->indexRelationName = "";

    this->joinBinaryOperator = NO_BINOP_CLAUSE;
    this->joinResultRelationName = "";
    this->joinFirstRelationName = "";
    this->joinSecondRelationName = "";
    this->joinFirstColumnName = "";
    this->joinSecondColumnName = "";

    this->loadRelationName = "";
    this->loadMatrixName = "";

    this->printRelationName = "";
    this->printMatrixName = "";

    this->projectionResultRelationName = "";
    this->projectionColumnList.clear();
    this->projectionRelationName = "";

    this->renameFromColumnName = "";
    this->renameToColumnName = "";
    this->renameRelationName = "";

    this->selectType = NO_SELECT_CLAUSE;
    this->selectionBinaryOperator = NO_BINOP_CLAUSE;
    this->selectionResultRelationName = "";
    this->selectionRelationName = "";
    this->selectionFirstColumnName = "";
    this->selectionSecondColumnName = "";
    this->selectionIntLiteral = 0;

    this->sortingStrategy = NO_SORT_CLAUSE;
    this->sortResultRelationName = "";
    this->sortColumnName = "";
    this->sortRelationName = "";

    this->sourceFileName = "";

    this->transposeMatrixName = "";
}

/**
 * @brief Checks to see if source file exists. Called when LOAD command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isFileExists(const string &tableName) {
    string fileName = "../data/" + tableName + ".csv";
    struct stat buffer{};
    return (stat(fileName.c_str(), &buffer) == 0);
}

/**
 * @brief Checks to see if source file exists. Called when SOURCE command is
 * invoked.
 *
 * @param tableName 
 * @return true 
 * @return false 
 */
bool isQueryFile(string fileName) {
    fileName = "../data/" + fileName + ".ra";
    struct stat buffer{};
    return (stat(fileName.c_str(), &buffer) == 0);
}
