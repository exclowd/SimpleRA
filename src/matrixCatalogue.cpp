//
// Created by exclowd on 9/10/21.
//
#include "global.h"
#include "matrixCatalogue.h"

void MatrixCatalogue::insertMatrix(Matrix *matrix) {
    logger.log("MatrixCatalogue::insertMatrix");
    this->matrices[matrix->matrixName] = matrix;
}

void MatrixCatalogue::deleteMatrix(const string &matrixName) {
    logger.log("MatrixCatalogue::deleteMatrx");
    this->matrices[matrixName]->unload();
    auto temp = this->matrices[matrixName];
    this->matrices.erase(matrixName);
    delete temp;
}

Matrix *MatrixCatalogue::getMatrix(const string &matrixName) {
    logger.log("MatrixCatalogue::getMatrix");
    Matrix *table = this->matrices[matrixName];
    return table;
}

bool MatrixCatalogue::isMatrix(const string &matrixName) {
    logger.log("MatrixCatalogue::isMatrix");
    if (this->matrices.count(matrixName))
        return true;
    return false;
}

void MatrixCatalogue::print() {
    logger.log("MatrixCatalogue::print");
    cout << "\nMATRICES" << endl;

    int rowCount = 0;
    for (const auto &rel: this->matrices) {
        cout << rel.first << endl;
        rowCount++;
    }
    printRowCount(rowCount);
}

MatrixCatalogue::~MatrixCatalogue() {
    logger.log("MatrixCatalogue::~MatrixCatalogue");
    for (const auto &matrix: this->matrices) {
        matrix.second->unload();
        delete matrix.second;
    }
}
