#pragma once

#include<iostream>
#include<bits/stdc++.h>
#include<sys/stat.h>
#include<fstream>

using namespace std;

class Logger {

    string logFile = "log";
    ofstream fout;

public:

    Logger();

    void log(const string& logString);
};

extern Logger logger;