#include "global.h"
#include "cursor.h"

#include <vector>

Cursor::Cursor(const string &tblName, int pgIndex) {
    logger->log("Cursor::Cursor");
    this->page = bufferManager->getPage(tblName, pgIndex);
    this->pagePointer = 0;
    this->tableName = tblName;
    this->pageIndex = pgIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext() {
    logger->log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if (result.empty()) {
        tableCatalogue->getTable(this->tableName)->getNextPage(this);
        if (!this->pagePointer) {
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

/**
 * @brief Function that loads Page indicated by pgIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pgIndex
 */
void Cursor::nextPage(int pgIndex) {
    logger->log("Cursor::nextPage");
    this->page = bufferManager->getPage(this->tableName, pgIndex);
    this->pageIndex = pgIndex;
    this->pagePointer = 0;
}
