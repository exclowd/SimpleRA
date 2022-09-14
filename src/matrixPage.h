#pragma once

#include "logger.h"
#include <vector>
#include <unordered_map>
#include "page.h"

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
class MatrixPage : public PageBase {
public:
    MatrixPage() {
        this->pageName = "";
        this->entityName = "";
        this->rowCount = 0;
        this->columnCount = 0;
        this->data.clear();
    }

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex);

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex, vector<vector<int>> _data, size_t rCount,
               size_t cCount);

    void writePage() override;

    vector<vector<int>> data;
};

class MatrixPageSparse : public PageBase {
public:
    MatrixPageSparse() {
        this->pageName = "";
        this->entityName = "";
        this->rowCount = 0;
        this->columnCount = 0;
        this->data.clear();
    }

    MatrixPageSparse(const string &matName, size_t pgIndex);

    MatrixPageSparse(const string &matName, size_t pgIndex, vector<tuple<size_t, size_t, int>> _data, size_t rCount);

    void writePage() override;

    vector<tuple<size_t, size_t, int>> data;
};
