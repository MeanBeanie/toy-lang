#pragma once
#include <string>
#include <map>
#include <vector>

enum TokenType {
	NUMBER = 0,
	STRING,
	IDENTIFIER,
	SUBTRACT,
	ADD = 4,
	DIVIDE,
	MULTIPLY,
	LT,
	LEQ = 8,
	GT,
	GEQ,
	EQ,
	ASSIGN = 12,
	LABEL,
	NEWLINE
};

struct Token {
	TokenType type;
	std::string raw;
};

class Lexer {
	public:
		Lexer(const char* path);

		std::vector<Token> getTokens(){ return tokens; }
	private:
		bool success = true;
		std::vector<Token> tokens;
		std::string buffer;
};

enum StatementType {
	VAR_INIT = 0,
	VAR_ASSIGN,
	MATH,
	COMPARISON,
	CONSTANT = 4,
	LABEL_IDENTIFIER,
	PRINT,
	GOTO,
	IF = 8,
	RETURN
};

struct Statement {
	StatementType type;
	std::vector<Token> tokens;
};

class Parser {
	public:
		Parser(std::vector<Token> tokens);

		std::vector<Statement> getStatements(){ return statements; }
	private:
		bool success = true;
		std::vector<Statement> statements;
};

enum VariableType {
	INT,
	STR
};

struct Variable {
	VariableType type;
	int number;
	std::string string;
};

class Interpreter {
	public:
		Interpreter(std::vector<Statement> statements);
	
	private:
		Statement simplify(Statement statement);
		std::map<std::string, Variable> vars;
		std::map<std::string, int> labels;
};
