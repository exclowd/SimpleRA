//
// Created by exclowd on 28/11/21.
//

#ifndef SIMPLERA_PARTITIONCATALOGUE_H
#define SIMPLERA_PARTITIONCATALOGUE_H

#include <vector>

#include "partition.h"
#include "table.h"

class PartitionCatalogue {
    vector<map<string, Partition *>> partitions;
    string entityName;
    string firstName;
    string secondName;

   public:
    PartitionCatalogue(string _entityName, size_t sz, Table *first, Table *second);

    ~PartitionCatalogue();

    Partition *getPartition(size_t partition, const string &tblName);

    void writePage(size_t partition, const string &tblName, HashPage *page);
};

#endif  // SIMPLERA_PARTITIONCATALOGUE_H
