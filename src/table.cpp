#include "global.h"

#include <algorithm>
#include <sstream>

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table() {
    logger->log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tblName
 */
Table::Table(const string &tblName) {
    logger->log("Table::Table");
    this->sourceFileName = "../data/" + tblName + ".csv";
    this->tableName = tblName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table entityName and the
 * c the table holds should be specified.
 *
 * @param tblName
 * @param c
 */
Table::Table(const string &tblName, const vector<string> &c) {
    logger->log("Table::Table");
    this->sourceFileName = "../data/temp/" + tblName + ".csv";
    this->tableName = tblName;
    this->columns = c;
    this->columnCount = c.size();
    this->distinctValuesInColumns.assign(this->columnCount, {});
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    this->maxRowsPerBlock = (size_t) ((BLOCK_SIZE * 1024) / (sizeof(int) * columnCount));
    this->writeRow<string>(c);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load() {
    logger->log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line)) {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column entityName
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(const string &firstLine) {
    logger->log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ',')) {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (size_t) ((BLOCK_SIZE * 1024) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify() {
    logger->log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> temp{};
    this->distinctValuesInColumns.assign(this->columnCount, temp);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line)) {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == (int) this->maxRowsPerBlock) {
            BufferManager::writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter) {
        BufferManager::writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row) {
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter])) {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(const string &columnName) {
    logger->log("Table::isColumn");
    return any_of(this->columns.begin(), this->columns.end(), [columnName](auto col) {
        return col == columnName;
    });
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(const string &fromColumnName, const string &toColumnName) {
    logger->log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
        if (columns[columnCounter] == fromColumnName) {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print() {
    logger->log("Table::print");
    size_t count = min(PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < (int) count; rowCounter++) {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor) const {
    logger->log("Table::getNext");

    if (cursor->pageIndex < (int) this->blockCount - 1) {
        cursor->nextPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent() {
    logger->log("Table::makePermanent");
    if (!this->isPermanent())
        BufferManager::deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < (int) this->rowCount; rowCounter++) {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent() const {
    logger->log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
        return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload() const {
    logger->log("Table::~unload");
    for (int pageCounter = 0; pageCounter < (int) this->blockCount; pageCounter++)
        bufferManager->deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        BufferManager::deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor() const {
    logger->log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}

/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(const string &columnName) {
    logger->log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < (int) this->columnCount; columnCounter++) {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
