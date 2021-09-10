#include <utility>

#include "global.h"
#include <vector>

/**
 * @brief Construct a new MatrixPage object. Never used as part of the code
 *
 */
MatrixPage::MatrixPage() {
    this->pageName = "";
    this->matrixName = "";
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new MatrixPage:: MatrixPage object given the matrix matrixName and page
 * index. When matrixs are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<matrixname>_Page<pageindex>". For example, If the MatrixPage being loaded is of
 * matrix "R" and the pgIndex is 2 then the file matrixName is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tblName
 * @param pgIndex
 */
MatrixPage::MatrixPage(const string &tblName, int pgIndex) {
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = tblName;
    std::to_string(pgIndex);
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pgIndex);
    Table matrix = *matrixCatalogue.getTable(tblName);
    this->columnCount = matrix.columnCount;
    size_t maxRowCount = matrix.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = matrix.rowsPerBlockCount[pgIndex];
    int number;
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        for (int columnCounter = 0; columnCounter < (int) columnCount; columnCounter++) {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
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
vector<int> MatrixPage::getRow(int rowIndex) {
    logger.log("MatrixPage::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= (int) this->rowCount)
        return result;
    return this->rows[rowIndex];
}

MatrixPage::MatrixPage(string tblName, size_t pgIndex, vector<vector<int>> _rows, int rCount) {
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = std::move(tblName);
    this->rows = _rows;
    this->rowCount = rCount;
    this->columnCount = (int) _rows[0].size();
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pgIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void MatrixPage::writePage() {
    logger.log("MatrixPage::writePage");
    ofstream fout(this->pageName, ios::trunc);
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
