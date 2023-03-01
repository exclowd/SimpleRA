#pragma once

#include <cassert>
#include <vector>

#include "executor.h"
#include "partitionCatalogue.h"

extern const size_t BLOCK_SIZE;
extern const size_t BLOCK_COUNT;
extern const size_t PRINT_COUNT;
extern vector<string> tokenizedQuery;
extern ParsedQuery* parsedQuery;
extern TableCatalogue* tableCatalogue;
extern MatrixCatalogue* matrixCatalogue;
extern PartitionCatalogue* partitionCatalogue;
extern BufferManager* bufferManager;
