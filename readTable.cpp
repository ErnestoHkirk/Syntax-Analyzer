#include "readTable.hpp"

void readTable::createMatrix(const char* filepath)
{
    std::ifstream inFile;

    try {
        inFile.open(filepath);
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLUMNS; j++) {
                inFile >> stateTable[i][j];
            }
        }
    }
    catch (std::exception& e) {
        std::cout << "ERROR: Could not read file.\n";
    }
}
