/*
 * PROJECT: Lexical Analyzer
 * 
 * Group Memebers:
 * Jacob Rapmund
 * Ernesto Hooghkirk
 * Andy Duong
 * Michael Lau
 * 
 * Professor: Anthony Le
 * 
 */

#include "FileReader.hpp"
#include "readTable.hpp"
#include "SyntaxAnalyzer.hpp"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <iomanip>

using namespace std;

//List of possible states for the state table
enum tokenState
{
    LETTER,
    DIGIT,
    DOLLAR,
    APOST,
    LEFTPAR,
    RIGHTPAR,
    LEFTCURL,
    RIGHTCURL,
    LEFTSQR,
    RIGHTSQR,
    COMMA,
    COLON,
    SEMICOLON,
    SPACE,
    PERIOD,
    EXCLA,
    OPER,
    BACKUP
};

//State matrix to be defined by calling the static function in readTable
int stateTable[ROWS][COLUMNS];

int eye = 0;
vector<string> syntaxOutput;
ofstream outfile2;

//struct for differentiating lexeme types
/*struct LexemePair
{
	std::string lexeme;
	std::string identifier;
};*/

//Creates a map that associates characters to states (key = char, value = tokenState)
std::unordered_map<char, tokenState> createStateMap();
//Creates a map to help differentiate between identifiers and keywords (key = string, value = bool)
std::unordered_map<std::string, bool> createKeySet();
//The tokenize function is a lexical analyzer. [s] is the source code string and [tokens] will be manipulated within the function to produce a vector of lexeme pairs
bool lexer(const std::string &s, std::vector<LexemePair> &tokens);
//Prints a set of lexeme pairs in an easy-to-read format on the console given a vector of [tokens]
void printLexerTable(const std::vector<LexemePair> &tokens);
//Syntax analyzer prototypes
bool analyzeSyntax(std::vector<LexemePair> &tokens); // function to start syntax analysis
// each function is for each rule
bool A(const std::vector<LexemePair> &tokens);
bool APrime(const std::vector<LexemePair> &tokens);
bool B(const std::vector<LexemePair> &tokens);
bool BPrime(const std::vector<LexemePair> &tokens);
bool C(const std::vector<LexemePair> &tokens);
bool D(const std::vector<LexemePair> &tokens);

void printSyntaxOutput();

void printAllSyntax();

int main()
{
    outfile2.open("outputPrdp.txt");

    std::vector<LexemePair> Tokens, Tokens2;
    cout << "==========================================================" << endl;
    cout << "Start of Lexical Analysis" << endl;
    cout << "==========================================================" << endl << endl;

    //extract and populate values for state matrix
    readTable::createMatrix("states.txt");
    //extract source code from file
    std::string fileData = FileReader::readFile("testfile.txt");
    std::string fileData2 = FileReader::readFile("testfile2.txt");

    //Perform lexical analysis
    if (!lexer(fileData, Tokens))
        printLexerTable(Tokens);
    else
        std::cout << "ERROR: Lexer could not analyze input file.";

    std::cout << "\nLexeme Set 2\n"
              << std::endl;
    if (!lexer(fileData2, Tokens2))
        printLexerTable(Tokens2);
    else
        std::cout << "ERROR: Lexer could not analyze input file.";

    cout << endl;
    cout << "==========================================================" << endl;
    cout << "End of Lexical Analysis" << endl;
    cout << "==========================================================" << endl << endl;

    // SYNTAX ANALYSIS
    cout << "==========================================================" << endl; 
    std::cout << "[Bottom-Up LR Syntax analysis.]" << std::endl;
    cout << "==========================================================" << endl << endl;

    SyntaxAnalyzer sa;
    sa.setPrintAnalysis(true);
    std::cout << (sa.analyze(Tokens2) ? "[input accepted.]\n" : "[input rejected.]\n") << std::endl;

    cout << "==========================================================" << endl;
    std::cout << "[End Syntax analysis (bottom-up).]" << std::endl;
    cout << "==========================================================" << endl << endl;

    sa.logToOutputFile();

    cout << "==========================================================" << endl;
    cout << "Start of Syntax Analysis (PRDP)." << endl;
    cout << "==========================================================" << endl << endl;

    if (analyzeSyntax(Tokens))
    {
        std::cout << "******** Accepted ********" << std::endl;
    }
    else
    {
        std::cout << "******** Not accepted ********" << std::endl;
        std::cout << "Invalid token and lexeme: \n";
        std::cout << "Identifier ====================== Lexeme" << endl;
        std::cout << Tokens[eye - 1].identifier << " | " << Tokens[eye - 1].lexeme << endl;
    }
    cout << endl;
    cout << "==========================================================" << endl;
    cout << "End of Syntax Analysis (PRDP)." << endl;
    cout << "==========================================================" << endl << endl;
    outfile2.close();

    //uncomment if console closes
    system("pause");
    return 0;
}

std::unordered_map<char, tokenState> createStateMap()
{
    std::unordered_map<char, tokenState> stateMap;

    stateMap['$'] = DOLLAR;
    stateMap['\''] = APOST;
    stateMap['('] = LEFTPAR;
    stateMap[')'] = RIGHTPAR;
    stateMap['{'] = LEFTCURL;
    stateMap['}'] = RIGHTCURL;
    stateMap['['] = LEFTSQR;
    stateMap[']'] = RIGHTSQR;
    stateMap[','] = COMMA;
    stateMap[':'] = COLON;
    stateMap[';'] = SEMICOLON;
    stateMap['\t'] = SPACE;
    stateMap['\n'] = SPACE;
    stateMap[' '] = SPACE;
    stateMap['\r'] = SPACE;
    stateMap['.'] = PERIOD;
    stateMap['!'] = EXCLA;
    stateMap['+'] = OPER;
    stateMap['-'] = OPER;
    stateMap['/'] = OPER;
    stateMap['*'] = OPER;
    stateMap['='] = OPER;
    stateMap['%'] = OPER;
    stateMap['>'] = OPER;
    stateMap['<'] = OPER;

    return stateMap;
}

std::unordered_map<std::string, bool> createKeySet()
{
    std::unordered_map<std::string, bool> keySet;

    keySet["int"] = true;
    keySet["float"] = true;
    keySet["bool"] = true;
    keySet["true"] = true;
    keySet["false"] = true;
    keySet["if"] = true;
    keySet["else"] = true;
    keySet["then"] = true;
    keySet["endif"] = true;
    keySet["while"] = true;
    keySet["whileend"] = true;
    keySet["do"] = true;
    keySet["doend"] = true;
    keySet["for"] = true;
    keySet["forend"] = true;
    keySet["input"] = true;
    keySet["output"] = true;
    keySet["and"] = true;
    keySet["or"] = true;
    keySet["not"] = true;

    return keySet;
}

void printLexerTable(const std::vector<LexemePair> &tokens)
{
    std::ofstream outfile;
    outfile.open("output.txt");
    outfile << std::left << std::setw(15) << "TOKENS"
            << "|" << std::right << std::setw(15) << "Lexemes"
            << "\n";
    outfile << std::setfill('=') << std::setw(31) << "" << std::setfill(' ') << std::endl;
    for (auto token : tokens)
        outfile << std::left << std::setw(15) << token.identifier << "=" << std::right << std::setw(15) << token.lexeme << std::endl;
    outfile.close();

    std::cout << std::left << std::setw(15) << "TOKENS"
              << "|" << std::right << std::setw(15) << "Lexemes"
              << "\n";
    std::cout << std::setfill('=') << std::setw(31) << "" << std::setfill(' ') << std::endl;
    for (auto token : tokens)
        std::cout << std::left << std::setw(15) << token.identifier << "=" << std::right << std::setw(15) << token.lexeme << std::endl;
}

bool lexer(const std::string &s, std::vector<LexemePair> &tokens)
{
    tokenState tState = LETTER;
    int state = 0;

    std::unordered_map<char, tokenState> stateMap = createStateMap();
    std::unordered_map<std::string, bool> keySet = createKeySet();

    std::string::const_iterator curr = s.cbegin();
    std::string::const_iterator pplace = s.cbegin();

    while (curr != s.cend())
    {
        //check next character
        if (std::isalpha(*curr))
            tState = LETTER;
        else if (std::isdigit(*curr))
            tState = DIGIT;
        else if (stateMap.find(*curr) != stateMap.end())
            tState = stateMap[*curr];
        else
            return 1; // because the character is not valid

        //update state table
        state = stateTable[state][tState] - 1;

        //check for valid state
        if (state == -1)
            return 1;

        //update character pointer
        curr++;

        //update pplace (useful for extra spaces)
        if (state == 0)
            pplace = curr;

        //check if accepting state and update
        if (state == 2 || state == 4 || state == 7 || state == 8 || state == 9 || state == 11 || curr == s.cend())
        {
            LexemePair npair;

            //backup character pointer if necessary
            if (stateTable[state][BACKUP])
                curr--;

            //eof check
            if (!(state == 2 || state == 4 || state == 7 || state == 8 || state == 9 || state == 11))
                state = stateTable[state][SPACE] - 1;

            //if the ending state was because we had a comment, skip creating a pair
            if (state != 7 && state != 11)
            {
                npair.lexeme = s.substr(pplace - s.cbegin(), curr - pplace);

                if (state == 2)
                {
                    //npair.identifier = (keySet.find(npair.lexeme) != keySet.end() ? "KEYWORD" : "IDENTIFIER");

                    if (keySet.find(npair.lexeme) != keySet.end())
                    {
                        npair.identifier = "KEYWORD";
                        npair.type = KEYWORD;
                    }
                    else
                    {
                        npair.identifier = "IDENTIFIER";
                        npair.type = IDENTIFIER;
                    }
                }
                else if (state == 4)
                {
                    npair.identifier = "INTEGER";
                    npair.type = INTEGER;
                }
                else if (state == 8)
                {
                    //npair.identifier = (tState == OPER ? "OPERATOR" : "SEPARATOR");

                    if (tState == OPER)
                    {
                        npair.identifier = "OPERATOR";
                        npair.type = OPERATOR;
                    }
                    else
                    {
                        npair.identifier = "SEPARATOR";
                        npair.type = SEPARATOR;
                    }
                }
                else if (state == 9)
                {
                    npair.identifier = "REAL";
                    npair.type = REAL;
                }

                if (npair.lexeme != "" && npair.identifier != "")
                    tokens.push_back(npair);
            }
            //revert back to the initial state and update pplace
            state = 0;
            pplace = curr;
        }
    }
    return 0;
}

/********************************************************
				CODE FOR PRDP
*********************************************************/
bool analyzeSyntax(std::vector<LexemePair> &tokens)
{
    LexemePair end;
    end.lexeme = "$";
    end.identifier = "a";

    tokens.push_back(end);
    if (A(tokens))
    {
        //std::cout << "Working!" << std::endl;
        return true;
    }
    // if assignment
    // if declarative
    return false;
}

// <Expression> -> <Term><Expression Prime>
bool A(const std::vector<LexemePair> &tokens)
{
    syntaxOutput.push_back("A -> BA'");
    if (B(tokens))
    {
        if (APrime(tokens))
        {
            return true;
        }
    }
    return false;
}

// <Expression Prime> -> +<Term><Expression Prime> || <Expression Prime> -> -<Term><Expression Prime>
bool APrime(const std::vector<LexemePair> &tokens)
{
    if (tokens[eye].lexeme == "+" || tokens[eye].lexeme == "-")
    {
        cout << endl;
        std::cout << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << std::endl;
        outfile2 << "\n"
                 << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << "\n";
        ++eye;
        syntaxOutput.push_back("A' -> +BA | A' -> -BA");
        printSyntaxOutput();
        if (B(tokens))
        {
            if (APrime(tokens))
            {
                printSyntaxOutput();
                return true;
            }
        }
    }
    else if (tokens[eye].lexeme == "$" || tokens[eye].lexeme == ")")
    {
        // follow set of A'
        syntaxOutput.push_back("A' -> epsilon");
        return true;
    }
    return false;
}

// <Term> -> <Factor><Term Prime>
bool B(const std::vector<LexemePair> &tokens)
{
    syntaxOutput.push_back("B -> CB'");
    if (C(tokens))
    {
        if (BPrime(tokens))
        {
            return true;
        }
    }
    return false;
}

// <Term Prime> -> *<Factor><Term Prime> || <Term Prime> -> /<Factor><Term Prime>
bool BPrime(const std::vector<LexemePair> &tokens)
{
    if (tokens[eye].lexeme == "*" || tokens[eye].lexeme == "/")
    {
        cout << endl;
        std::cout << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << std::endl;
        outfile2 << "\n"
                 << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << "\n";
        ++eye;
        syntaxOutput.push_back("B' -> *CB' | /CB'");

        printSyntaxOutput();
        if (C(tokens))
        {
            if (BPrime(tokens))
            {
                return true;
            }
        }
    }
    else if (tokens[eye].lexeme == "+" || tokens[eye].lexeme == "-" || tokens[eye].lexeme == "$" || tokens[eye].lexeme == ")")
    {
        syntaxOutput.push_back("B' -> epsilon");
        printSyntaxOutput();
        return true;
    }
    return false;
}

// <Factor> -> (Expression) || <Factor> -> <ID>
bool C(const std::vector<LexemePair> &tokens)
{
    if (tokens[eye].lexeme == "(")
    {
        std::cout << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << endl;
        outfile2 << "\n"
                 << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << "\n";
        syntaxOutput.push_back("C -> (");
        printSyntaxOutput();
        eye++;
        if (A(tokens))
        {
            if (tokens[eye].lexeme == ")")
            {
                std::cout << std::endl
                          << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << endl;
                outfile2 << "\n"
                         << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << "\n";
                syntaxOutput.push_back("C -> )");
                printSyntaxOutput();
                eye++;
                return true;
            }
        }
    }
    if (tokens[eye].identifier == "IDENTIFIER")
    {
        syntaxOutput.push_back("C -> D");
        if (D(tokens))
        {
            return true;
        }
    }
    return false;
}

// <ID> -> id
bool D(const std::vector<LexemePair> &tokens)
{
    syntaxOutput.push_back("D -> id");
    if (tokens[eye].identifier == "IDENTIFIER")
    {
        cout << endl;
        std::cout << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << endl;
        outfile2 << "\n"
                 << "Token: " << tokens[eye].identifier << "  Lexeme: " << tokens[eye].lexeme << "\n";
        ++eye;
        printSyntaxOutput();
        return true;
    }
    return false;
}

void printSyntaxOutput()
{
    for (int i = 0; i < syntaxOutput.size(); i++)
    {
        std::cout << syntaxOutput[i] << std::endl;
    }
    printAllSyntax();
    syntaxOutput.clear();
}

void printAllSyntax()
{
    for (int i = 0; i < syntaxOutput.size(); i++)
    {
        outfile2 << syntaxOutput[i];
        outfile2 << "\n";
    }
}