#pragma once

#include "logger.h"
#include "page.h"
#include <vector>

/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class HashPage : public PageBase {

public:
    string tableName;

    HashPage();

    HashPage(const string &rsltName, size_t partition, const string &tblName, size_t pgIndex);

    HashPage(const string &tblName, const string &firstName, size_t partition, size_t pgIndex, int cCount);

    void addRow(const vector<int> &row);

    void writePage() override;

    ~HashPage() override = default;

    vector<vector<int>> rows{};
};
