//
// Created by exclowd on 9/2/21.
//
#include <sstream>
#include <algorithm>
#include "matrix.h"

Matrix::Matrix(const string &matName) {
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matName + ".csv";
    this->matrixName = matName;
}


bool Matrix::load() {
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line)) {
        fin.close();
        if (this->extractColumnSize(line))
            this->blockify();
    }
    fin.close();
    return false;
}

bool Matrix::extractColumnSize(const string& firstline) {
    logger.log("Matrix:extractColumnSize");
    string word;
    stringstream s(firstline);
    while (getline(s, word, ',')) {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (word.empty()) {
            return false;
        }
    }
    auto cnt = count(firstline.begin(), firstline.end(), ',');
    this->size = cnt;
    return true;
}


