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

    void insertmatrix(Matrix *matrix);

    void deletematrix(const string &matrixName);

    Matrix *getmatrix(const string &matrixName);

    bool isMatrix(const string &matrixName);

    void print();

    ~MatrixCatalogue();
};
