#pragma once

#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType {
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORTMATRIX,
    EXPORT,
    GROUPBY,
    INDEX,
    JOIN,
    LIST,
    LISTMATRIX,
    LOAD,
    LOADMATRIX,
    PRINT,
    PRINTMATRIX,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    TRANSPOSE,
    UNDETERMINED
};

enum BinaryOperator {
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy {
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType {
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

class ParsedQuery {

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName;

    string crossResultRelationName;
    string crossFirstRelationName;
    string crossSecondRelationName;

    string distinctResultRelationName;
    string distinctRelationName;

    string exportRelationName;
    string exportMatrixName;

    string groupByGroupingAttributeName;
    string groupByResultRelationName;
    string groupByRelationName;
    string groupByOperatorName;
    string groupByAttributeName;

    IndexingStrategy indexingStrategy = IndexingStrategy::NOTHING;
    string indexColumnName;
    string indexRelationName;

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName;
    string joinFirstRelationName;
    string joinSecondRelationName;
    string joinFirstColumnName;
    string joinSecondColumnName;

    string loadRelationName;
    string loadMatrixName;

    string printRelationName;
    string printMatrixName;

    string projectionResultRelationName;
    vector<string> projectionColumnList;
    string projectionRelationName;

    string renameFromColumnName;
    string renameToColumnName;
    string renameRelationName;

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName;
    string selectionRelationName;
    string selectionFirstColumnName;
    string selectionSecondColumnName;
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName;
    string sortColumnName;
    string sortRelationName;
    int sortBufferSize;

    string transposeMatrixName;

    string sourceFileName;

    ParsedQuery();

    void clear();
};

bool syntacticParse();

bool syntacticParseCLEAR();

bool syntacticParseCROSS();

bool syntacticParseDISTINCT();

bool syntacticParseEXPORT();

bool syntacticParseEXPORTMATRIX();

bool syntacticParseGROUPBY();

bool syntacticParseINDEX();

bool syntacticParseJOIN();

bool syntacticParseLIST();

bool syntacticParseLISTMATRIX();

bool syntacticParseLOAD();

bool syntacticParseLOADMATRIX();

bool syntacticParsePRINT();

bool syntacticParsePRINTMATRIX();

bool syntacticParsePROJECTION();

bool syntacticParseRENAME();

bool syntacticParseSELECTION();

bool syntacticParseSORT();

bool syntacticParseSOURCE();

bool syntacticParseTRANSPOSE();

bool isFileExists(const string &tableName);

bool isQueryFile(string fileName);

