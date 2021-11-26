#include <utility>

#include "global.h"
#include "matrixPage.h"
#include <vector>
#include <cassert>

/**
 * @brief Construct a new MatrixPage:: MatrixPage object given the matrix entityName and page
 * index. When matrixs are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<matrixname>_Page<pageindex>". For example, If the MatrixPage being loaded is of
 * matrix "R" and the pgIndex is 2 then the file entityName is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tblName
 * @param pgIndex
 */
MatrixPage::MatrixPage(const string &matName, size_t rowIndex, size_t colIndex) {
    logger->log("MatrixPage::MatrixPage");
    this->entityName = matName;
    this->pageName = "../data/temp/" + this->entityName + "_MPage" + to_string(rowIndex) + ":" + to_string(colIndex);
    Matrix matrix = *matrixCatalogue->getMatrix(matName);
    this->rowCount = matrix.rowsPerBlockCount[rowIndex];
    this->columnCount = matrix.columnsPerBlockCount[colIndex];
    this->data.assign(rowCount, vector<int>(columnCount));
    ifstream fin(pageName, ios::in);
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
            int number;
            fin >> number;
            this->data[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}


MatrixPage::MatrixPage(const string &matName, size_t rowIndex, size_t colIndex,
                       vector<vector<int>> _data, size_t rCount, size_t cCount) {
    logger->log("MatrixPage::MatrixPage");
    this->entityName = matName;
    this->data = std::move(_data);
    this->rowCount = rCount;
    this->columnCount = cCount;
    this->pageName = "../data/temp/" + this->entityName + "_MPage" + to_string(rowIndex) + ":" + to_string(colIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void MatrixPage::writePage() {
    logger->log("MatrixPage::writePage");
    logger->debug("Write page to disk");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
            if (columnCounter != 0)
                fout << " ";
            fout << this->data[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}


/**
 * SPARSE MATRIX
 */

/**
 * @brief Construct a new MatrixPage:: MatrixPage object given the matrix entityName and page
 * index. When matrixs are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<matrixname>_MSPage<rowIndex>:<colIndex>". For example, If the MatrixPage being loaded is of
 * matrix "R" and the pgIndex is 2 then the file entityName is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tblName
 * @param pgIndex
 */
MatrixPageSparse::MatrixPageSparse(const string &matName, size_t pgIndex) {
    logger->log("MatrixPageSparse::MatrixPageSparse");
    this->entityName = matName;
    this->pageName = "../data/temp/" + this->entityName + "_MSPage" + to_string(pgIndex);
    Matrix matrix = *matrixCatalogue->getMatrix(matName);
    this->rowCount = matrix.rowsPerBlockCount[pgIndex];
    assert(matrix.columnsPerBlockCount[pgIndex] == 3);
    this->columnCount = matrix.columnsPerBlockCount[pgIndex];
    ifstream fin(pageName, ios::in);
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        int x, y, z;
        fin >> x >> y >> z;
        this->data.emplace_back(x, y, z);
    }
    fin.close();
}


MatrixPageSparse::MatrixPageSparse(const string &matName, size_t pgIndex, vector<tuple<size_t, size_t, int>> _data,
                                   size_t rCount) {
    logger->log("MatrixPageSparse::MatrixPageSparse");
    this->entityName = matName;
    this->data = std::move(_data);
    this->rowCount = rCount;
    this->columnCount = 3;
    this->pageName = "../data/temp/" + this->entityName + "_MSPage" + to_string(pgIndex);
}

/**
 * @brief writes current page contents to file.
 *
 */
void MatrixPageSparse::writePage() {
    logger->log("MatrixPageSparse::writePage");
    logger->debug("Write page to disk");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        fout << get<0>(this->data[rowCounter]) << " " << get<1>(this->data[rowCounter]) << " "
             << get<2>(this->data[rowCounter]) << "\n";
    }
    fout.close();
}
