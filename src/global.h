#pragma once

#include "executor.h"
#include <vector>

extern float BLOCK_SIZE;
extern size_t BLOCK_COUNT;
extern size_t PRINT_COUNT;
extern vector<string> tokenizedQuery;
extern ParsedQuery parsedQuery;
extern TableCatalogue tableCatalogue;
extern MatrixCatalogue matrixCatalogue;
extern BufferManager bufferManager;