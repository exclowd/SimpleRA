#pragma once

#include "executor.h"
#include <vector>

extern const size_t BLOCK_SIZE;
extern const size_t BLOCK_COUNT;
extern const size_t PRINT_COUNT;
extern vector<string> tokenizedQuery;
extern ParsedQuery parsedQuery;
extern TableCatalogue tableCatalogue;
extern MatrixCatalogue matrixCatalogue;
extern BufferManager bufferManager;