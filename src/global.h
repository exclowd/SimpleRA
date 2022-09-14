#pragma once

#include "executor.h"
#include "partitionCatalogue.h"
#include <vector>
#include <cassert>

extern const size_t BLOCK_SIZE;
extern const size_t BLOCK_COUNT;
extern const size_t PRINT_COUNT;
extern vector<string> tokenizedQuery;
extern ParsedQuery* parsedQuery;
extern TableCatalogue* tableCatalogue;
extern MatrixCatalogue* matrixCatalogue;
extern PartitionCatalogue* partitionCatalogue;
extern BufferManager* bufferManager;
