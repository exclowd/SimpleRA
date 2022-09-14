#pragma once

#include<iostream>
#include<fstream>

using namespace std;

class Logger {

    string logFile = "log";
    ofstream fout;
    int times = 0;

public:
    Logger();

    void debug(const string &logString);

    void log(const string &logString);
};

extern Logger *logger;