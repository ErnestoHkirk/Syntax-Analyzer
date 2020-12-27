
GXX49_VERSION := $(shell g++-4.9 --version 2>/dev/null)

ifdef GXX49_VERSION
	CXX_COMMAND := g++-4.9
else
	CXX_COMMAND := g++
endif

CXX = ${CXX_COMMAND} -std=c++14 -Wall

run_compiler: compiler
	./compiler
	 
headers: FileReader.hpp readTable.hpp SyntaxAnalyzer.hpp
 
compiler: headers main.cpp
	${CXX} FileReader.cpp readTable.cpp SyntaxAnalyzer.cpp main.cpp -o compiler
	
clean: 
	rm -f compiler
