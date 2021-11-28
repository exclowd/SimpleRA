#include <utility>

#include "global.h"
#include "partition.h"
#include "hashPage.h"

#include <vector>

/**
 * @brief Construct a new HashPage object. Never used as part of the code
 *
 */
HashPage::HashPage() {
    this->tableName = "";
    this->entityName = "";
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new HashPage:: HashPage object given the table entityName and HashPage
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_HashPage<HashPageindex>". For example, If the HashPage being loaded is of
 * table "R" and the pgIndex is 2 then the file entityName is "R_HashPage2". The HashPage
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tblName
 * @param pgIndex
 */
HashPage::HashPage(const string &rsltName, size_t partition, const string &tblName, size_t pgIndex) {
    logger->log("HashPage::HashPage");
    this->entityName = tblName;
    this->tableName = "../data/temp/" + rsltName + to_string(partition) + "_HPage" + this->entityName + "_" +
                      to_string(pgIndex);
    logger->debug("Read HashPage: " + this->tableName + " from disk ");
    Partition *entry = partitionCatalogue->getPartition(partition, tblName);
    this->columnCount = entry->columnCount;
    size_t maxRowCount = entry->maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    ifstream fin(this->tableName, ios::in);
    this->rowCount = entry->rowsPerBlockCount[pgIndex];
    this->rows.resize(rowCount);
    int number;
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        vector<int> temp(columnCount);
        for (int columnCounter = 0; columnCounter < (int) columnCount; columnCounter++) {
            fin >> temp[columnCounter];
        }
        this->rows[rowCounter] = std::move(temp);
    }
    fin.close();
}

HashPage::HashPage(const string &tblName, const string &firstName, size_t partition, size_t pgIndex, int cCount) {
    logger->log("HashPage::HashPage");
    this->columnCount = cCount;
    this->rowCount = 0;
    this->entityName = firstName;
    this->tableName = "../data/temp/" + tblName + to_string(partition) + "_HPage" + this->entityName + "_" +
                      to_string(pgIndex);
}

/**
 * @brief writes current HashPage contents to file.
 * 
 */
void HashPage::writePage() {
    logger->log("HashPage::writeHashPage");
    logger->debug("Write HashPage: " + this->tableName + " to disk ");
    ofstream fout(this->tableName, ios::trunc);
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

void HashPage::addRow(const vector<int> &row) {
    this->rowCount++;
    this->rows.push_back(row);
}
