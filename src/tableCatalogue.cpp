#include <utility>

#include "global.h"

void TableCatalogue::insertTable(Table *table) {
    logger.log("TableCatalogue::insertTable");
    this->tables[table->tableName] = table;
}

void TableCatalogue::deleteTable(const string &tableName) {
    logger.log("TableCatalogue::deleteTable");
    this->tables[tableName]->unload();
    delete this->tables[tableName];
    this->tables.erase(tableName);
}

Table *TableCatalogue::getTable(const string &tableName) {
    logger.log("TableCatalogue::getTable");
    Table *table = this->tables[tableName];
    return table;
}

bool TableCatalogue::isTable(const string &tableName) {
    logger.log("TableCatalogue::isTable");
    if (this->tables.count(tableName))
        return true;
    return false;
}

bool TableCatalogue::isColumnFromTable(const string &columnName, const string &tableName) {
    logger.log("TableCatalogue::isColumnFromTable");
    if (this->isTable(tableName)) {
        Table *table = this->getTable(tableName);
        if (table->isColumn(columnName))
            return true;
    }
    return false;
}

void TableCatalogue::print() {
    logger.log("TableCatalogue::print");
    cout << "\nRELATIONS" << endl;

    int rowCount = 0;
    for (const auto &rel: this->tables) {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

TableCatalogue::~TableCatalogue() {
    logger.log("TableCatalogue::~TableCatalogue");
    for (const auto &table: this->tables) {
        table.second->unload();
        delete table.second;
    }
}
