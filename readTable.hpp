#ifndef READTABLE_HPP
#define READTABLE_HPP

#include <fstream>
#include <iostream>

#define ROWS 12
#define COLUMNS 18

extern int stateTable[ROWS][COLUMNS];

class readTable
{
    private:
        readTable() {};
    public:
        static void createMatrix(const char* filepath);
};
#endif
