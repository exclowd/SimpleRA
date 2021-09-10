#include <utility>

#include "global.h"
#include "matrixPage.h"
#include <vector>

/**
 * @brief Construct a new MatrixPage object. Never used as part of the code
 *
 */
template<>
MatrixPage<vector<vector<int>>>::MatrixPage() {
    this->pageName = "";
    this->entityName = "";
    this->rowCount = 0;
    this->columnCount = 0;
    this->data.clear();
}

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
template<>
MatrixPage<vector<vector<int>>>::MatrixPage(const string &matName, size_t rowIndex, size_t colIndex) {
    logger.log("MatrixPage::MatrixPage");
    this->entityName = matName;
    this->pageName = "../data/temp/" + this->entityName + "_MPage" + to_string(rowIndex) + ":" + to_string(colIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matName);
    this->rowCount = matrix.rowsPerBlockCount[rowIndex];
    this->columnCount = matrix.columnsPerBlockCount[columnCount];
    size_t maxCount = matrix.maxDimPerBlock;
    vector<int> row(columnCount, 0);
    this->data.assign(rowCount, row);
    ifstream fin(pageName, ios::in);
    this->rowCount = matrix.rowsPerBlockCount[rowIndex];
    int number;
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
            fin >> number;
            this->data[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}

/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
template<>
auto MatrixPage<vector<vector<int>>>::getData() {
    logger.log("MatrixPage::getData");
    return this->data;
}

template<>
MatrixPage<vector<vector<int>>>::MatrixPage(const string &matName, size_t rowIndex, size_t colIndex,
                                            vector<vector<int>> _data, size_t rCount, size_t cCount) {
    logger.log("MatrixPage::MatrixPage");
    this->entityName = matName;
    this->data = std::move(_data);
    this->rowCount = rCount;
    this->columnCount = cCount;
    this->pageName = "../data/temp/" + this->entityName + "_Page" + to_string(rowIndex) + ":" + to_string(colIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
template<>
void MatrixPage<vector<vector<int>>>::writePage() {
    logger.log("MatrixPage::writePage");
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
