//
// Created by exclowd on 9/2/21.
//
#pragma once

#include "cursor.h"

class Matrix {
    Matrix();

    bool blockify();

    bool load();
    void print();
    void makePermanent();
    [[nodiscard]] bool isPermanent() const;
    [[nodiscard]] Cursor getCursor() const;
    void unload() const;

private:
    bool extractColumnSize(const string&);

    string sourceFileName;
    string matrixName;
    size_t size;
    size_t blockCount;

};

