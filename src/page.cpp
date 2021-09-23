#include <utility>

#include "global.h"
#include <vector>

/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page() {
    this->pageName = "";
    this->entityName = "";
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table entityName and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pgIndex is 2 then the file entityName is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tblName
 * @param pgIndex
 */
Page::Page(const string &tblName, size_t pgIndex) {
    logger.log("Page::Page");
    this->entityName = tblName;
    this->pageName = "../data/temp/" + this->entityName + "_Page" + to_string(pgIndex);
    Table table = *tableCatalogue.getTable(tblName);
    this->columnCount = table.columnCount;
    size_t maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);
    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pgIndex];
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
vector<int> Page::getRow(int rowIndex) {
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= (int) this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tblName, size_t pgIndex, vector<vector<int>> _rows, int rCount) {
    logger.log("Page::Page");
    this->entityName = std::move(tblName);
    this->rows = _rows;
    this->rowCount = rCount;
    this->columnCount = (int) _rows[0].size();
    this->pageName = "../data/temp/" + this->entityName + "_Page" + to_string(pgIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage() {
    logger.log("Page::writePage");
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
