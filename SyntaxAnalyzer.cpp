#include "SyntaxAnalyzer.hpp"
#include "FileReader.hpp"
#include <exception>
#include <iomanip>
#include <fstream>

const char* ESOURCETABLE = "SyntaxEStates.txt";
const char* SSOURCETABLE = "statementsStates.txt";

int SyntaxAnalyzer::getPreviousState(std::string currentStack)
{
	std::string digits = "0123456789";
	std::string::size_type found = currentStack.find_last_not_of(digits);
	return std::stoi(currentStack.substr(found + 1));
}

bool SyntaxAnalyzer::reduce( std::vector<ReductionRule>& reductionList, std::vector<std::vector<StateRule>>& rules, std::unordered_map<char, int>& cols, StateRule& rule, std::string& currentstack)
{
	try
	{
		if(this->printAnalysis) std::cout << "[REDUCTION] = " << reductionList[rule.location].reduction << " -> " << reductionList[rule.location].rule << std::endl;

    //test
    output << "[REDUCTION] = " << reductionList[rule.location].reduction << " -> " << reductionList[rule.location].rule << std::endl;

		while (currentstack.back() != reductionList[rule.location].rule[0])
			currentstack.pop_back();
		currentstack.pop_back();

		int previousState = getPreviousState(currentstack);

		int nextState = rules[previousState][ cols[reductionList[rule.location].reduction] ].location;
		currentstack.push_back(reductionList[rule.location].reduction);
		currentstack += std::to_string(nextState);
	}
	catch (std::exception e)
	{
		return false;
	}
	return true;
}

void SyntaxAnalyzer::initialize_maps()
{
	//create the map of terminals and nonterminals for the equation grammar
	std::unordered_map<char, int> equation_t_map_init;

	equation_t_map_init['i'] = 0;
	equation_t_map_init['n'] = 1;
	equation_t_map_init['r'] = 2;
	equation_t_map_init['+'] = 3;
	equation_t_map_init['-'] = 4;
	equation_t_map_init['*'] = 5;
	equation_t_map_init['/'] = 6;
	equation_t_map_init['('] = 7;
	equation_t_map_init[')'] = 8;
	equation_t_map_init[';'] = 9;
	equation_t_map_init['E'] = 10;
	equation_t_map_init['T'] = 11;
	equation_t_map_init['F'] = 12;
	equation_t_map_init['I'] = 13;
	equation_t_map_init['N'] = 14;

	this->ExpressionCols = equation_t_map_init;

	equation_t_map_init.clear();
	equation_t_map_init['d'] = 0;
	equation_t_map_init['i'] = 1;
	equation_t_map_init['b'] = 2;
	equation_t_map_init['f'] = 3;
	equation_t_map_init['='] = 4;
	equation_t_map_init['E'] = 5;
	equation_t_map_init[';'] = 6;
	equation_t_map_init['S'] = 7;
	equation_t_map_init['A'] = 8;
	equation_t_map_init['D'] = 9;
	equation_t_map_init['P'] = 10;
	equation_t_map_init['I'] = 11;

	this->StatementCols = equation_t_map_init;

	//create the array (string) of reductions
	this->reductionListE.push_back({ 'S', "E;" });
	this->reductionListE.push_back({ 'E',"E + T" });
	this->reductionListE.push_back({ 'E',"E - T" });
	this->reductionListE.push_back({ 'E',"T" });
	this->reductionListE.push_back({ 'T',"T * F" });
	this->reductionListE.push_back({ 'T',"T / F" });
	this->reductionListE.push_back({ 'T',"F" });
	this->reductionListE.push_back({ 'F',"( E )" });
	this->reductionListE.push_back({ 'F',"I" });    
	this->reductionListE.push_back({ 'F',"N" });  
	this->reductionListE.push_back({ 'I',"i (identifier)" }); //identifiers
	this->reductionListE.push_back({ 'N',"n (integer)" }); //integers
	this->reductionListE.push_back({ 'N',"real" }); //reals

	this->reductionListS.push_back({ 'Z', "S;" });
	this->reductionListS.push_back({ 'S',"A" });
	this->reductionListS.push_back({ 'S',"D" });
	this->reductionListS.push_back({ 'A',"I = E" });
	this->reductionListS.push_back({ 'D',"P I" });
	this->reductionListS.push_back({ 'P',"bool" });
	this->reductionListS.push_back({ 'P',"float" });
	this->reductionListS.push_back({ 'P',"int" });
	this->reductionListS.push_back({ 'I',"d (identifier)" });
}

Action SyntaxAnalyzer::translateAction(char actionCode)
{
	switch (actionCode)
	{
	case 's':
		return SHIFT;
	case 'r':
		return REDUCTION;
	case 'v':
		return EVALUATE;
	case 'e': default:
		return ERROR;
	}
}

bool SyntaxAnalyzer::analyzeE(std::vector<LexemePair>::iterator& it, std::vector<LexemePair>& tokens)
{
	std::string stack = "0";
	char type = ' ';
	StateRule rule;

	if (this->printAnalysis) std::cout << "Current Stack: " << stack << std::endl;
  output << "Current Stack: " << stack << std::endl;

	while (it != tokens.end())
	{
		int previousState = getPreviousState(stack);

		if (this->printAnalysis) std::cout << "[TOKEN] = " << it->identifier << "\t[LEXEME] = " << it->lexeme << std::endl;

    output << "[TOKEN] = " << it->identifier << "\t[LEXEME] = " << it->lexeme << std::endl;

		switch (it->type)
		{
		case IDENTIFIER:
			type = 'i';
			break;
		case OPERATOR: case SEPARATOR:
			type = it->lexeme[0];
			break;
		case INTEGER:
			type = 'n';
			break;
		case REAL:
			type = 'r';
			break;
		default:
			return false;
		}

		rule = this->Erules[previousState][this->ExpressionCols[type]];

		switch (rule.action)
		{
		case SHIFT:
			stack.push_back(type);
			stack += std::to_string(rule.location);
			it++;
			break;
		case REDUCTION:
			if (!reduce(reductionListE, Erules, ExpressionCols, rule, stack)) return false;
		break;
		case ACCEPT:
			if (this->printAnalysis) std::cout << "\n[expression accepted.]\n" << std::endl;
      output << "\n[expression accepted.]\n" << std::endl;
			return true;
			break;
		default:
			return false;
		}
		if(this->printAnalysis) std::cout << "Current Stack: " << stack << std::endl;
    output << "Current Stack: " << stack << std::endl;
	}
	return false;
}

void SyntaxAnalyzer::setPrintAnalysis(bool printAnalysis)
{
	this->printAnalysis = printAnalysis;
}

void SyntaxAnalyzer::loadRules(const char* path, std::vector<std::vector<StateRule>>& ruleset)
{
	std::string data = FileReader::readFile(path);

	std::stringstream ss;
	ss << data;

	std::string stateInfo;

	while (std::getline(ss, stateInfo))
	{
		std::stringstream line;
		line << stateInfo;

		std::vector<StateRule> row;
		while (line >> stateInfo)
		{
			StateRule rule;

			if (stateInfo.find(',') != std::string::npos)
			{
				rule.action = translateAction(stateInfo[0]);
				rule.location = std::stoi(stateInfo.substr(2));
			}
			else if (stateInfo == "acc")
			{
				rule.action = ACCEPT;
				rule.location = -1;
			}
			else
			{
				rule.action = STANDBY;
				rule.location = std::stoi(stateInfo);
			}

			row.push_back(rule);
		}
		ruleset.push_back(row);
		row.clear();
	}
}

SyntaxAnalyzer::SyntaxAnalyzer()
	:printAnalysis(false)
{
	this->initialize_maps();

	//load rules from text files
	loadRules(ESOURCETABLE, this->Erules);
	loadRules(SSOURCETABLE, this->Srules);

  //printRules();
}

void SyntaxAnalyzer::printRules()
{
	std::cout << "Expression Rules:\n\n";
  output << "Expression Rules:\n\n";
	for (auto row : Erules)
	{
		for (auto rule : row)
		{
			std::cout << rule.action << " " << std::setw(2) << rule.location << "\t";
      output << rule.action << " " << std::setw(2) << rule.location << "\t";
		}
		std::cout << std::endl;
    output << std::endl;
	}

	std::cout << "\nStatement Rules:\n\n";
  output << "\nStatement Rules:\n\n";
	for (auto row : Srules)
	{
		for (auto rule : row)
		{
			std::cout << rule.action << " " << std::setw(2) << rule.location << "\t";
      output << rule.action << " " << std::setw(2) << rule.location << "\t";
		}
		std::cout << std::endl;
    output << std::endl;
	}
}

bool SyntaxAnalyzer::analyze(std::vector<LexemePair> tokens)
{
	std::string stack = "0";
	char type = ' ';

	std::vector<LexemePair>::iterator it = tokens.begin();
	StateRule rule;

	if (this->printAnalysis) std::cout << "Current Stack: " << stack << std::endl;
  output << "Current Stack: " << stack << std::endl;

	while (it != tokens.end())
	{
		int previousState = getPreviousState(stack);

		if (this->printAnalysis) std::cout << "[TOKEN] = " << it->identifier << "\t[LEXEME] = " << it->lexeme << std::endl;

    output << "[TOKEN] = " << it->identifier << "\t[LEXEME] = " << it->lexeme << std::endl;

    
		//special condition for expression
		if (previousState != 10)
		{
			switch (it->type)
			{
			case IDENTIFIER:
				type = 'd';
				break;
			case OPERATOR: case SEPARATOR:
				type = it->lexeme[0];
				//std::cout << type << std::endl;
				break;
			case KEYWORD:
				if (it->lexeme == "int") type = 'i';
				else if (it->lexeme == "float") type = 'f';
				else if (it->lexeme == "bool") type = 'b';
				break;
			default:
				return false;
			}
		}
		else type = 'E';

		rule = this->Srules[previousState][this->StatementCols[type]];

		switch (rule.action)
		{
		case SHIFT:
			stack.push_back(type);
			stack += std::to_string(rule.location);
			it++;
			break;
		case REDUCTION:
			if (!reduce(reductionListS, Srules, StatementCols, rule, stack)) return false;
		break;
		case EVALUATE:
			//code for expression evaluation
			if (this->printAnalysis) std::cout << "\n[evaluating expression.]\n" << std::endl;
      output << "\n[evaluating expression.]\n" << std::endl;
			if (this->analyzeE(it, tokens))
			{
				stack.push_back(type);
				stack += std::to_string(rule.location);
			}
			else return false;
			break;
		case ACCEPT:
			it++;
			if(this->printAnalysis) std::cout << "\n[line accepted.]\n" << std::endl;
      output << "\n[line accepted.]\n" << std::endl;
			if (it != tokens.end())
			{
				if(this->printAnalysis) std::cout << "[scanning next line.]\n" << std::endl;
        output << "[scanning next line.]\n" << std::endl;
				stack = "0";
			}
			else
				return true;
			break;
		default:
			return false;
		}
		if(this->printAnalysis) std::cout << "Current Stack: " << stack << std::endl;
    output << "Current Stack: " << stack << std::endl;
	}
	return false;
}

void SyntaxAnalyzer::logToOutputFile(std::string file)
{
  std::fstream outFile;

  try
  {
    outFile.open(file);
    outFile << output.rdbuf();
    outFile.close();
  }
  catch(std::exception e)
  {
    std::cout << "Unable to print log to output file." << std::endl;
  }
}