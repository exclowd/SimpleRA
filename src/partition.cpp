//
// Created by exclowd on 28/11/21.
//

#include "partition.h"
#include "global.h"

#include <utility>

Partition::Partition(string _tableName, const vector<string> &c) : tableName(std::move(_tableName)), columns(c) {
    this->columnCount = c.size();
    this->maxRowsPerBlock = (size_t) ((BLOCK_SIZE * 1024) / (sizeof(int) * columnCount));
}

void Partition::addPage(HashPage *p) {
    assert(this->tableName == p->entityName);
    this->blockCount++;
    this->rowsPerBlockCount.push_back(p->rowCount);
}

