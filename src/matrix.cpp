//
// Created by exclowd on 9/2/21.
//
#include <sstream>
#include <algorithm>
#include "matrix.h"
#include "global.h"

Matrix::Matrix(const string &matName) {
    logger->log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matName + ".csv";
    this->matrixName = matName;
    this->maxDimPerBlock = floor(sqrt((1024 / sizeof(int)) * BLOCK_SIZE));
}

bool Matrix::load() {
    logger->log("Matrix::load");
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
    logger->log("Matrix:extractSize");
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
    logger->log("Matrix::blockify");
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
    if (this->isSparse) {
        auto nz = this->size * this->size - zero;
        this->maxDimPerBlock = (size_t) ((BLOCK_SIZE * 1024) / (sizeof(int) * 3));
        while (nz >= this->maxDimPerBlock) {
            nz -= this->maxDimPerBlock;
            this->rowsPerBlockCount.push_back(this->maxDimPerBlock);
        }
        if (nz > 0) this->rowsPerBlockCount.push_back(nz);
        this->columnsPerBlockCount = vector<size_t>(this->rowsPerBlockCount.size(), 3);
        size_t pageCounter = 0;
        vector<tuple<size_t, size_t, int>> rowsInPage;
        ifstream filein(this->sourceFileName, ios::in);
        this->blockCount = 0;
        for (size_t i = 0; i < this->size; i++) {
            getline(filein, line);
            std::stringstream ss(line);
            std::string cell;
            for (size_t j = 0; j < this->size; j++) {
                if (!getline(ss, cell, ','))
                    return false;
                auto p = stoi(cell);
                if (p == 0) continue;
                rowsInPage.emplace_back(i, j, p);
                pageCounter++;
                if (pageCounter == this->rowsPerBlockCount[this->blockCount]) {
                    BufferManager::writePageSparse(this->matrixName, this->blockCount, rowsInPage, pageCounter);
                    this->blockCount++;
                    pageCounter = 0;
                    rowsInPage.clear();
                }
            }
        }
        if (this->rowsPerBlockCount.size() != this->blockCount)
            return false;
        return true;
    } else {
        auto cnt = this->size;
        while (cnt >= this->maxDimPerBlock) {
            cnt -= this->maxDimPerBlock;
            this->columnsPerBlockCount.push_back(this->maxDimPerBlock);
        }
        if (cnt > 0) this->columnsPerBlockCount.push_back(cnt);
        if (this->columnsPerBlockCount.size() != this->blockCount)
            return false;
        this->rowsPerBlockCount = this->columnsPerBlockCount;
        for (size_t j = 0; j < this->blockCount; j++) {
            ifstream filein(this->sourceFileName, ios::in);
            for (size_t i = 0; i < this->blockCount; i++) {
                cout << "loading " << i << " " << j << "\r";
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
}

void Matrix::transpose() {
    logger->log("Matrix::transpose");
    if (this->isSparse) {
        for (size_t pgIndex = 0; pgIndex < this->blockCount; pgIndex++) {
            auto p1 = bufferManager->getPageSparse(this->matrixName, pgIndex);
            for (auto&[x, y, z]: p1.data) {
                swap(x, y);
            }
            sort(p1.data.begin(), p1.data.end());
            BufferManager::writePageSparse(this->matrixName, pgIndex, p1.data, this->rowsPerBlockCount[pgIndex]);
            bufferManager->updatePageSparse(this->matrixName, pgIndex);
        }
    } else {
        for (size_t r = 0; r < this->blockCount; r++) {
            for (size_t c = r + 1; c < this->blockCount; c++) {
                auto p1 = bufferManager->getPage(this->matrixName, r, c);
                auto p2 = bufferManager->getPage(this->matrixName, c, r);
                for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                    for (size_t j = 0; j < this->columnsPerBlockCount[c]; j++) {
                        swap(p1.data[i][j], p2.data[j][i]);
                    }
                }
                BufferManager::writePage(this->matrixName, r, c, p1.data, this->rowsPerBlockCount[r],
                                         this->columnsPerBlockCount[c]);
                BufferManager::writePage(this->matrixName, c, r, p2.data, this->rowsPerBlockCount[c],
                                         this->columnsPerBlockCount[r]);
                bufferManager->updatePage(this->matrixName, r, c);
                bufferManager->updatePage(this->matrixName, c, r);
            }
        }
        for (size_t r = 0; r < this->blockCount; r++) {
            auto p1 = bufferManager->getPage(this->matrixName, r, r);
            for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                for (size_t j = i + 1; j < this->columnsPerBlockCount[r]; j++) {
                    swap(p1.data[i][j], p1.data[j][i]);
                }
            }
            BufferManager::writePage(this->matrixName, r, r, p1.data, this->rowsPerBlockCount[r],
                                     this->columnsPerBlockCount[r]);
            bufferManager->updatePage(this->matrixName, r, r);
        }
    }
}

struct hash_pair {
    template<class T1, class T2>
    size_t operator()(const pair<T1, T2> &p) const {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};

void Matrix::print() const {
    logger->log("Matrix::print");
    if (this->isSparse) {
        unordered_map<pair<size_t, size_t>, pair<int, int>, hash_pair> mp;
        vector<size_t> pgPtr(this->blockCount, 0);
        for (size_t pgIndex = 0; pgIndex < this->blockCount; pgIndex++) {
            auto p = bufferManager->getPageSparse(this->matrixName, pgIndex);
            auto[a, b, c] = p.data[pgPtr[pgIndex]++];
            mp[{a, b}] = {c, pgIndex};
        }
        auto getVal = [&](size_t x, size_t y) {
            if (mp.count({x, y}) != 0) {
                auto[val, pgIndex] = mp[{x, y}];
                mp.erase({x, y});
                auto p = bufferManager->getPageSparse(this->matrixName, pgIndex);
                if (pgPtr[pgIndex] != p.data.size()) {
                    auto[a, b, c] = p.data[pgPtr[pgIndex]++];
                    mp[{a, b}] = {c, pgIndex};
                }
                return val;
            }
            return 0;
        };
        for (size_t i = 0; i < min(this->size, 20ul); i++) {
            for (size_t j = 0; j < this->size; j++) {
                if (j != 0) cout << ",";
                cout << getVal(i, j);
            }
            cout << endl;
        }
    } else {
        size_t i = 0;
        for (size_t r = 0; r < this->blockCount; r++) {
            for (; i < this->rowsPerBlockCount[r] && i < 20; i++) {
                size_t j = 0;
                for (size_t c = 0; c < this->blockCount; c++) {
                    auto p = bufferManager->getPage(this->matrixName, r, c);
                    for (; j < this->columnsPerBlockCount[c] && j < 20; j++) {
                        if (j != 0) cout << ",";
                        cout << p.data[i][j];
                    }
                }
                cout << endl;
            }
        }
    }
}


void Matrix::makePermanent() {
    logger->log("Matrix::makePermanent");
    if (!this->isPermanent())
        BufferManager::deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    if (this->isSparse) {
        unordered_map<pair<size_t, size_t>, pair<int, int>, hash_pair> mp;
        vector<size_t> pgPtr(this->blockCount, 0);
        for (size_t pgIndex = 0; pgIndex < this->blockCount; pgIndex++) {
            auto p = bufferManager->getPageSparse(this->matrixName, pgIndex);
            auto[a, b, c] = p.data[pgPtr[pgIndex]++];
            mp[{a, b}] = {c, pgIndex};
        }
        auto getVal = [&](size_t x, size_t y) {
            if (mp.count({x, y}) != 0) {
                auto[val, pgIndex] = mp[{x, y}];
                mp.erase({x, y});
                auto p = bufferManager->getPageSparse(this->matrixName, pgIndex);
                if (pgPtr[pgIndex] != p.data.size()) {
                    auto[a, b, c] = p.data[pgPtr[pgIndex]++];
                    mp[{a, b}] = {c, pgIndex};
                }
                return val;
            }
            return 0;
        };
        for (size_t i = 0; i < this->size; i++) {
            for (size_t j = 0; j < this->size; j++) {
                if (j != 0) fout << ",";
                fout << getVal(i, j);
            }
            fout << endl;
        }
    } else {
        for (size_t r = 0; r < this->blockCount; r++) {
            for (size_t i = 0; i < this->rowsPerBlockCount[r]; i++) {
                for (size_t c = 0; c < this->blockCount; c++) {
                    auto p = bufferManager->getPage(this->matrixName, r, c);
                    for (size_t j = 0; j < this->columnsPerBlockCount[c]; j++) {
                        if (c != 0 || j != 0) fout << ',';
                        fout << p.data[i][j];
                    }
                }
                fout << endl;
            }
        }
    }
}

bool Matrix::isPermanent() const {
    logger->log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

void Matrix::unload() const {
    logger->log("Matrix::unload");
    if (this->isSparse) {
        for (size_t i = 0; i < this->blockCount; i++) {
            bufferManager->deleteFileSparse(this->matrixName, i);
        }
    } else {
        for (size_t i = 0; i < this->blockCount; i++) {
            for (size_t j = 0; j < this->blockCount; j++) {
                bufferManager->deleteFile(this->matrixName, i, j);
            }
        }
    }
}


