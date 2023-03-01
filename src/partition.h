//
// Created by exclowd on 28/11/21.
//

#ifndef SIMPLERA_PARTITION_H
#define SIMPLERA_PARTITION_H

#include <map>
#include <string>
#include <vector>

#include "hashPage.h"

struct Partition {
    explicit Partition(string _tableName, const vector<string> &c);

    ~Partition() = default;

    void addPage(HashPage *p);

    vector<string> columns;
    string tableName;
    size_t blockCount{};
    size_t columnCount{};
    size_t maxRowsPerBlock{};
    vector<size_t> rowsPerBlockCount{};
};

#endif  // SIMPLERA_PARTITION_H
