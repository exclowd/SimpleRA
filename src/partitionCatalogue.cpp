//
// Created by exclowd on 28/11/21.
//

#include "partitionCatalogue.h"

#include <cassert>

PartitionCatalogue::PartitionCatalogue(string _entityName, size_t sz, Table *first, Table *second) : entityName(std::move(_entityName)),
                                                                                                     partitions(sz),
                                                                                                     firstName(first->tableName),
                                                                                                     secondName(second->tableName) {
    logger->log("PartitionCatalogue::PartitionCatalogue");
    for (auto &x : this->partitions) {
        x[first->tableName] = new Partition(first->tableName, first->columns);
        x[second->tableName] = new Partition(second->tableName, second->columns);
    }
}

PartitionCatalogue::~PartitionCatalogue() {
    logger->log("PartitionCatalogue::~PartitionCatalogue");
    for (auto &x : this->partitions) {
        for (auto &[k, v] : x) {
            delete v;
        }
    }
}

Partition *PartitionCatalogue::getPartition(size_t partition, const string &tblName) {
    logger->log("PartitionCatalogue::getPartition");
    assert(tblName == firstName || tblName == secondName);
    return this->partitions[partition][tblName];
}

void PartitionCatalogue::writePage(size_t partition, const string &tblName, HashPage *page) {
    logger->log("PartitionCatalogue::writePage");
    Partition *p = this->getPartition(partition, tblName);
    p->addPage(page);
    page->writePage();
    delete page;
}
