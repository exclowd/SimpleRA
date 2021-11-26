#include "logger.h"

Logger::Logger() {
    this->fout.open(this->logFile, ios::out);
}

void Logger::log(const string &logString) {
    fout << logString << endl;
}

void Logger::debug(const string &logString) {
    cerr << "[DEBUG "  << times++ << "]: "  << logString << endl;
}
