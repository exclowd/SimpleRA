//
// Created by exclowd on 9/2/21.
//
#include <sstream>
#include <algorithm>
#include "matrix.h"
#include "global.h"

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
        if (this->extractSize(line))
            return this->blockify();
    }
    fin.close();
    return false;
}

bool Matrix::extractSize(const string &firstline) {
    logger.log("Matrix:extractSize");
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

bool Matrix::blockify() {
    logger.log("Matrix::blockify");
    fstream fin(this->sourceFileName, ios::in);

    return false;
}

void Matrix::transpose() {

}

void Matrix::print() const {

}

void Matrix::makePermanent() {
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        BufferManager::deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);


}

bool Matrix::isPermanent() const {
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void Matrix::unload() const {
    logger.log("Matrix::unload");
    for (long long int i = 0; i < this->blockCount; i++) {
        for (long long int j = 0; j < this->blockCount; j++) {
            bufferManager.deleteFile(this->matrixName, i, j);
        }
    }
}


