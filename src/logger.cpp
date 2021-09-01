#include "global.h"

Logger::Logger() {
    this->fout.open(this->logFile, ios::out);
}

void Logger::log(const string &logString) {
    fout << logString << endl;
}
