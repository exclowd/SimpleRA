//
// Created by exclowd on 28/11/21.
//

#include "partitionCatalogue.h"
#include <cassert>

PartitionCatalogue::PartitionCatalogue(string _entityName, size_t sz, Table *first, Table *second) : entityName(
        std::move(_entityName)), partitions(sz), firstName(first->tableName), secondName(second->tableName) {
    for (auto &x: this->partitions) {
        x[first->tableName] = new Partition(first->tableName, first->columns);
        x[second->tableName] = new Partition(second->tableName, second->columns);
    }
}

PartitionCatalogue::~PartitionCatalogue() {
    for (auto &x: this->partitions) {
        for (auto[k, v]: x) {
            delete v;
        }
    }
}

Partition *PartitionCatalogue::getPartition(size_t partition, const string &tblName) {
    assert(tblName == firstName || tblName == secondName);
    return this->partitions[partition][tblName];
}

void PartitionCatalogue::writePage(size_t partition, const string &tblName, HashPage *page) {
    Partition *p = this->getPartition(partition, tblName);
    p->addPage(page);
    page->writePage();
    delete page;
}


