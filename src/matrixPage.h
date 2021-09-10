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


template<typename T>
class MatrixPage : public PageBase {
public:
    using storage_type = T;

    MatrixPage();

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex);

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex, size_t rCount, size_t cCount);

    void writePage();

private:
    string matrixName;
    size_t size;
    storage_type data;
};
