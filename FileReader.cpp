#include "FileReader.hpp"

std::string FileReader::readFile(const char* filepath)
{
	std::ifstream inFile;
	inFile.exceptions(std::ios::failbit | std::ios::badbit);
	
	try
	{
		std::stringstream ss;

		inFile.open(filepath);
		ss << inFile.rdbuf();
		inFile.close();

		return ss.str();
	}
	catch (std::exception& e)
	{
		std::cout << "ERROR: Could not read file.\n";
	}

	return "";
}