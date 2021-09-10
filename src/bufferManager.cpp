#include <utility>
#include <vector>
#include <algorithm>

#include "global.h"
#include "bufferManager.h"


BufferManager::BufferManager() {
    logger.log("BufferManager::BufferManager");
}



/**
 * @brief Checks to see if a page exists in the pool
 *
 * @param pageName 
 * @return true 
 * @return false 
 */
bool BufferManager::inPool(const string &pageName) {
    logger.log("BufferManager::inPool");
    return any_of(this->pages.begin(), this->pages.end(), [&](auto x) {
        return x.pageName == pageName;
    });
}

/**
 * @brief If the page is present in the pool, then this function returns the
 * page. Note that this function will fail if the page is not present in the
 * pool.
 *
 * @param pageName 
 * @return Page 
 */
Page BufferManager::getFromPool(const string &pageName) {
    logger.log("BufferManager::getFromPool");
    for (auto page: this->pages)
        if (pageName == page.pageName)
            return page;
}

/**
 * @brief Inserts page indicated by entityName and pageIndex into pool. If the
 * pool is full, the pool ejects the oldest inserted page from the pool and adds
 * the current page at the end. It naturally follows a queue data structure. 
 *
 * @param tableName 
 * @param pageIndex 
 * @return Page 
 */
Page BufferManager::insertIntoPool(const string &tableName, size_t pageIndex) {
    logger.log("BufferManager::insertIntoPool");
    Page page(tableName, pageIndex);
    if (this->pages.size() >= BLOCK_COUNT)
        pages.pop_front();
    pages.push_back(page);
    return page;
}

/**
 * @brief Deletes file names fileName
 *
 * @param fileName
 */
void BufferManager::deleteFile(const string &fileName) {

    if (remove(fileName.c_str()))
        logger.log("BufferManager::deleteFile: Err");
    else logger.log("BufferManager::deleteFile: Success");
}

/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName
 * @param pageIndex
 * @return Page
 */
Page BufferManager::getPage(const string &tableName, size_t pageIndex) {
    logger.log("BufferManager::getPage");
    string pageName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(tableName, pageIndex);
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param tableName 
 * @param pageIndex 
 * @param rows 
 * @param rowCount 
 */
void BufferManager::writePage(string tableName, size_t pageIndex, vector<vector<int>> rows, int rowCount) {
    logger.log("BufferManager::writePage");
    Page page(std::move(tableName), pageIndex, std::move(rows), rowCount);
    page.writePage();
}


/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the entityName and pageIndex.
 *
 * @param tableName 
 * @param pageIndex 
 */
void BufferManager::deleteFile(const string &tableName, int pageIndex) {
    logger.log("BufferManager::deleteFile");
    string fileName = "../data/temp/" + tableName + "_Page" + to_string(pageIndex);
    this->deleteFile(fileName);
}


/**
 * @brief Function called to read a page from the buffer manager. If the page is
 * not present in the pool, the page is read and then inserted into the pool.
 *
 * @param tableName
 * @param pageIndex
 * @return Page
 */
Page BufferManager::getMatrixPage(const string &matrixName, size_t rowIndex, size_t colIndex) {
    logger.log("BufferManager::getMatrixPage");
    string pageName = "../data/temp/" + matrixName + "_MPage" + to_string(rowIndex) + ":" + to_string(colIndex);
    if (this->inPool(pageName))
        return this->getFromPool(pageName);
    else
        return this->insertIntoPool(matrixName, pageIndex);
}

/**
 * @brief The buffer manager is also responsible for writing pages. This is
 * called when new tables are created using assignment statements.
 *
 * @param matrixName
 * @param pageIndex
 * @param rows
 * @param rowCount
 */
void BufferManager::writeMatrixPage(string matrixName, size_t rowIndex, size_t colIndex, vector<vector<int>> data) {
    logger.log("BufferManager::writeMatrixPage");
    MatrixPage page(std::move(matrixName), rowIndex, colIndex, std::move(data));
    page.writePage();
}


/**
 * @brief Overloaded function that calls deleteFile(fileName) by constructing
 * the fileName from the matrixName and pageIndex.
 *
 * @param matrixName
 * @param pageIndex
 */
void BufferManager::deleteMatrixFile(const string &matrixName, int rowIndex, int colIndex) {
    logger.log("BufferManager::deleteMatrixFile");
    string fileName = "../data/temp/" + matrixName + "_MPage" + to_string(rowIndex) + ":" + to_string(colIndex);
    this->deleteFile(fileName);
}

