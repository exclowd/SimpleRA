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


using matrix_hash_t = unordered_map<pair<size_t, size_t>, int>;
using matrix_data_t = vector<vector<int>>;

template<typename T>
class MatrixPage : public PageBase {
public:
    static_assert(std::is_same_v<T, matrix_hash_t> || std::is_same_v<T, matrix_data_t>);

    MatrixPage();

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex);

    MatrixPage(const string &matName, size_t rowIndex, size_t colIndex, vector<vector<int>> _data, size_t rCount, size_t cCount);

    auto getData();

    void writePage();

private:
    T data;
};
