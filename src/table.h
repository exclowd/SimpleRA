#pragma once

#include "cursor.h"
#include <unordered_set>


enum class IndexingStrategy {
    BTREE,
    HASH,
    NOTHING
};

/**
 * @brief The Table class holds all information related to a loaded table. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. There are typically 2 ways a table object gets
 * created through the course of the workflow - the first is by using the LOAD
 * command and the second is to use assignment statements (SELECT, PROJECT,
 * JOIN, SORT, CROSS and DISTINCT). 
 *
 */
class Table {
    vector<unordered_set<int>> distinctValuesInColumns;

public:
    string sourceFileName;
    string tableName;
    vector<string> columns;
    vector<size_t> distinctValuesPerColumnCount;
    size_t columnCount = 0;
    size_t rowCount = 0;
    size_t blockCount = 0;
    size_t maxRowsPerBlock = 0;
    vector<size_t> rowsPerBlockCount;
    bool indexed = false;
    string indexedColumn;
    IndexingStrategy indexingStrategy = IndexingStrategy::NOTHING;

    bool extractColumnNames(const string &firstLine);

    bool blockify();

    void updateStatistics(vector<int> row);

    Table();

    explicit Table(const string &tblName);

    Table(const string &tblName, const vector<string> &c);

    bool load();

    bool isColumn(const string &columnName);

    void renameColumn(const string &fromColumnName, const string &toColumnName);

    void print();

    void makePermanent();

    [[nodiscard]] bool isPermanent() const;

    void getNextPage(Cursor *cursor) const;

    [[nodiscard]] Cursor getCursor() const;

    int getColumnIndex(const string &columnName);

    void unload() const;

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template<typename T>
    void writeRow(vector<T> row, ostream &fout) {
        logger.log("Table::writeRow");
        for (int columnCounter = 0; columnCounter < (int) row.size(); columnCounter++) {
            if (columnCounter != 0)
                fout << ", ";
            fout << row[columnCounter];
        }
        fout << endl;
    }

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template<typename T>
    void writeRow(vector<T> row) {
        logger.log("Table::writeRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};