//
// Created by exclowd on 9/2/21.
//
#include <sstream>
#include <algorithm>
#include <cassert>
#include "matrix.h"
#include "global.h"

Matrix::Matrix(const string &matName) {
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matName + ".csv";
    this->matrixName = matName;
    this->maxDimPerBlock = floor(sqrt((1024 / sizeof(int)) * BLOCK_SIZE));
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
        if (word.empty()) {
            return false;
        }
    }
    auto cnt = count(firstline.begin(), firstline.end(), ',');
    if (firstline.back() == ',') cnt--;
    this->size = cnt + 1;
    this->blockCount = (this->size + this->maxDimPerBlock - 1) / this->maxDimPerBlock;
    return true;
}

bool Matrix::blockify() {
    logger.log("Matrix::blockify");
    fstream fin(this->sourceFileName, ios::in);
    // decide whether sparse or not
    size_t zero{};
    string line;
    for (size_t i = 0; i < this->size; i++) {
        getline(fin, line);
        std::stringstream ss(line);
        std::string cell;
        for (size_t j = 0; j < this->size; j++) {
            if (!getline(ss, cell, ','))
                return false;
            auto p = stoi(cell);
            if (p == 0LL) zero++;
        }
    }
    if ((double) zero >= 0.6 * (double) this->size * (double) this->size) this->isSparse = true;
    fin.close();
    auto cnt = this->size;


    while (cnt >= this->maxDimPerBlock) {
        cnt -= this->maxDimPerBlock;
        this->columnsPerBlockCount.push_back(this->maxDimPerBlock);
    }
    if (cnt > 0) this->columnsPerBlockCount.push_back(cnt);
    assert(this->columnsPerBlockCount.size() == this->blockCount);
    this->rowsPerBlockCount = this->columnsPerBlockCount;
    if (this->isSparse) {

    } else {
        for (size_t j = 0; j < this->blockCount; j++) {
            ifstream filein(this->sourceFileName, ios::in);
            for (size_t i = 0; i < this->blockCount; i++) {
                vector<vector<int>> data(this->rowsPerBlockCount[i]);
                for (size_t row = 0; row < this->rowsPerBlockCount[i]; row++) {
                    getline(filein, line);
                    stringstream ss(line);
                    string word;
                    size_t col;
                    // seek to appropriate column number
                    for (col = 0; col < j * this->maxDimPerBlock; col++) {
                        if (!getline(ss, word, ',')) {
                            return false;
                        }
                    }
                    vector<int> rowdata;
                    for (col = 0; col < this->columnsPerBlockCount[j]; col++) {
                        if (!getline(ss, word, ',')) {
                            return false;
                        }
                        rowdata.push_back(stoi(word));
                    }
                    data[row] = rowdata;
                }
                BufferManager::writePage(this->matrixName, i, j, data, this->rowsPerBlockCount[i],
                                         this->columnsPerBlockCount[j]);
            }
        }
        return true;
    }
    return false;
}

void Matrix::transpose() {
    logger.log("Matrix::transpose");
    if (this->isSparse) {

    } else {
        cout << "Hello" << endl;
        for (size_t r = 0; r < this->blockCount; r++) {
            for (size_t c = r + 1; c < this->blockCount; c++) {
                auto p1 = bufferManager.getPage(this->matrixName, r, c);
                auto p2 = bufferManager.getPage(this->matrixName, c, r);
                for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                    for (size_t j = 0; j < this->columnsPerBlockCount[c]; j++) {
                        swap(p1.data[i][j], p2.data[j][i]);
                    }
                }
                BufferManager::writePage(this->matrixName, r, c, p2.data, this->rowsPerBlockCount[r],
                                         this->columnsPerBlockCount[c]);
                BufferManager::writePage(this->matrixName, c, r, p1.data, this->rowsPerBlockCount[c],
                                         this->columnsPerBlockCount[r]);
                bufferManager.updatePage(this->matrixName, r, c);
                bufferManager.updatePage(this->matrixName, c, r);
            }
        }
        for (size_t r = 0; r < this->blockCount; r++) {
            auto p1 = bufferManager.getPage(this->matrixName, r, r);
            for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                for (size_t j = i + 1; j < this->columnsPerBlockCount[r]; j++) {
                    swap(p1.data[i][j], p1.data[j][i]);
                }
            }
            BufferManager::writePage(this->matrixName, r, r, p1.data, this->rowsPerBlockCount[r],
                                     this->columnsPerBlockCount[r]);
            cout << "before update" << endl;
            bufferManager.updatePage(this->matrixName, r, r);
            cout << "done update" << endl;
        }
    }
}

void Matrix::print() const {
    logger.log("Matrix::print");
    if (this->isSparse) {

    } else {
        for (size_t r = 0; r < this->blockCount; r++) {
            for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                for (size_t c = 0; c < this->blockCount; c++) {
                    auto p = bufferManager.getPage(this->matrixName, r, c);
                    cout << p.data[i][0];
                    for (size_t j = 1; j < this->columnsPerBlockCount[c]; j++) {
                        cout << "," << p.data[i][j];
                    }
                }
                cout << endl;
            }
        }
    }
}

void Matrix::makePermanent() {
    logger.log("Matrix::makePermanent");
    if (!this->isPermanent())
        BufferManager::deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    if (this->isSparse) {

    } else {
        for (size_t r = 0; r < this->blockCount; r++) {
            for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                for (size_t c = 0; c < this->blockCount; c++) {
                    auto p = bufferManager.getPage(this->matrixName, r, c);
                    fout << p.data[i][0];
                    for (size_t j = 1; j < this->columnsPerBlockCount[c]; j++) {
                        fout << "," << p.data[i][j];
                    }
                }
                fout << endl;
            }
        }
    }
}

bool Matrix::isPermanent() const {
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void Matrix::unload() const {
    logger.log("Matrix::unload");
    if (this->isSparse) {

    } else {
        for (size_t i = 0; i < this->blockCount; i++) {
            for (size_t j = 0; j < this->blockCount; j++) {
                bufferManager.deleteFile(this->matrixName, i, j);
            }
        }
    }
}


