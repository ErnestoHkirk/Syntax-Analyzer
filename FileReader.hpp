#ifndef FILEREADER_HPP
#define FILEREADER_HPP

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class FileReader
{
private:
    FileReader() {};
public:
    static std::string readFile(const char* filepath);
};

#endif