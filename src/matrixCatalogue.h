//
// Created by exclowd on 9/10/21.
//

#pragma once


#include "matrix.h"
#include <unordered_map>

class MatrixCatalogue {

    unordered_map<string, Matrix *> matrices;

public:
    MatrixCatalogue() = default;

    void insertMatrix(Matrix *matrix);

    void deleteMatrix(const string &matrixName);

    Matrix *getMatrix(const string &matrixName);

    bool isMatrix(const string &matrixName);

    void print();

    ~MatrixCatalogue();
};
