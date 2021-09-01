#pragma once

#include "semanticParser.h"

void executeCommand();

void executeCLEAR();

void executeCROSS();

void executeDISTINCT();

void executeEXPORT();

void executeINDEX();

void executeJOIN();

void executeLIST();

void executeLOAD();

void executePRINT();

void executePROJECTION();

void executeRENAME();

void executeSELECTION();

void executeSORT();

void executeSOURCE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);

template<typename T>
void printRowCount(T rowCount) {
    cout << "\n\nRow Count: " << rowCount << endl;
}