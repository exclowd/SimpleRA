// Server Code
#include <regex>

#include "global.h"

using namespace std;

const size_t BLOCK_SIZE = 1;
const size_t BLOCK_COUNT = 10;
const size_t PRINT_COUNT = 20;

Logger *logger;
vector<string> tokenizedQuery;
ParsedQuery *parsedQuery;
TableCatalogue *tableCatalogue;
MatrixCatalogue *matrixCatalogue;
BufferManager *bufferManager;
PartitionCatalogue *partitionCatalogue;

void doCommand() {
    logger->log("doCommand");
    if (syntacticParse() && semanticParse())
        executeCommand();
}

int main() {
    regex delim("[^\\s,]+");
    string command;
    if (system("rm -rf ../data/temp") < 0) {
        return -1;
    }
    if (system("mkdir ../data/temp") < 0) {
        return -1;
    };

    logger = new Logger;
    tableCatalogue = new TableCatalogue;
    matrixCatalogue = new MatrixCatalogue;
    bufferManager = new BufferManager;
    partitionCatalogue = nullptr;
    while (!cin.eof()) {
        cout << "\n> ";
        tokenizedQuery.clear();
        parsedQuery = new ParsedQuery;
        logger->log("\nReading New Command: ");
        getline(cin, command);
        logger->log(command);

        auto words_begin = std::sregex_iterator(command.begin(), command.end(), delim);
        auto words_end = std::sregex_iterator();
        for (std::sregex_iterator i = words_begin; i != words_end; ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (tokenizedQuery.size() == 1 && tokenizedQuery.front() == "QUIT") {
            break;
        }

        if (tokenizedQuery.empty()) {
            continue;
        }

        if (tokenizedQuery.size() == 1) {
            cout << "SYNTAX ERROR" << endl;
            continue;
        }

        doCommand();

        delete parsedQuery;
    }

    delete tableCatalogue;
    delete matrixCatalogue;

    bufferManager->cleanUp();
    delete bufferManager;

    delete logger;

    return 0;
}