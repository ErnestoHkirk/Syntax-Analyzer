#ifndef SYNTAXANALYZER_HPP
#define SYNTAXANALYZER_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <sstream>

enum lexemeType { KEYWORD, IDENTIFIER, INTEGER, OPERATOR, SEPARATOR, REAL };

struct LexemePair
{
	std::string lexeme;
	std::string identifier;
	lexemeType type;
};

enum Action
{
	SHIFT = 0,
	REDUCTION,
	STANDBY,
	ERROR,
	ACCEPT,
	EVALUATE
};

struct StateRule
{
	Action action;
	int location;
};

struct ReductionRule
{
	char reduction;
	std::string rule;
};

class SyntaxAnalyzer
{
private:
	//columns for accessing the state tables
	std::unordered_map<char, int> ExpressionCols;
	std::unordered_map<char, int> StatementCols;
	//the state tables
	std::vector<std::vector<StateRule>> Erules;
	std::vector<std::vector<StateRule>> Srules;
	//the reduction rules and return values
	std::vector<ReductionRule> reductionListE;
	//ReductionRule reductionListS[9];
	std::vector<ReductionRule> reductionListS;
	bool printAnalysis;
  std::stringstream output;

	int getPreviousState(std::string currentStack);
	bool reduce(std::vector<ReductionRule>& reductionList, std::vector<std::vector<StateRule>>& rules, std::unordered_map<char, int>& cols, StateRule& rule, std::string& currentstack);

	void initialize_maps();
	Action translateAction(char actionCode);
	void loadRules(const char* path, std::vector<std::vector<StateRule>>& ruleset);

public:
	SyntaxAnalyzer();

	//show rules
	void printRules();

	//analyze everything
	bool analyze(std::vector<LexemePair> tokens);
	//analyze expressions
	bool analyzeE(std::vector<LexemePair>::iterator& it, std::vector<LexemePair>& tokens);

	void setPrintAnalysis(bool printAnalysis);

  void logToOutputFile(std::string file = "output.txt");
};

#endif