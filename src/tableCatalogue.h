#include "table.h"

/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
class TableCatalogue {
    unordered_map<string, Table *> tables;

public:
    TableCatalogue() {}

    void insertTable(Table *table);

    void deleteTable(const string& tableName);

    Table *getTable(const string& tableName);

    bool isTable(const string& tableName);

    bool isColumnFromTable(string columnName, const string& tableName);

    void print();

    ~TableCatalogue();
};