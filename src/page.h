#pragma once

#include "logger.h"
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

class PageBase {
public:
    string pageName;

    size_t rowCount;
    size_t columnCount;

    string entityName;

    virtual void writePage() = 0;

    virtual ~PageBase() = default;
};

class Page : public PageBase {

public:
    Page();

    Page(const string &tblName, size_t pgIndex);

    Page(string tblName, size_t pgIndex, vector<vector<int>> _rows, int rCount);

    vector<int> getRow(int rowIndex);

    void writePage() override;

    ~Page() override = default;

private:
    vector<vector<int>> rows{};
};
