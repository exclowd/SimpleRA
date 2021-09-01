#pragma once

#include<iostream>
#include<fstream>

using namespace std;

class Logger {

    string logFile = "log";
    ofstream fout;

public:
    Logger();

    void log(const string &logString);
};

extern Logger logger;