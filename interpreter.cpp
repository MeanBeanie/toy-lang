#include "interpreter.hpp"
#include <iostream>
#include <fstream>

Lexer::Lexer(const char* path){
	// get characters;
	std::ifstream file(path);
	std::vector<char> buffer;
	char c;
	while(file >> std::noskipws >> c){
		buffer.push_back(c);
	}
	file.close();

	// convert raw characters to tokens;
	bool inString = false;
	Token current;
	for(int i = 0; i < buffer.size(); i++){
		if(buffer[i] == '"'){
			inString = !inString;
		}

		if(!inString && buffer[i] == ' ' || buffer[i] == '\t' || buffer[i] == '\n'){
			if(current.raw.size() == 0){
				continue;
			}

			if(current.raw.size() == 1){
				if(isalpha(current.raw[0])){
					current.type = IDENTIFIER;
				}
				else{
					switch(current.raw[0]){
						case '+':
						{
							current.type = ADD;
							break;
						}
						case '-':
						{
							current.type = SUBTRACT;
							break;
						}
						case '*':
						{
							current.type = MULTIPLY;
							break;
						}
						case '/':
						{
							current.type = DIVIDE;
							break;
						}
						case '>':
						{
							current.type = GT;
							break;
						}
						case '<':
						{
							current.type = LT;
							break;
						}
						case '=':
						{
							current.type = ASSIGN;
							break;
						}
						case '0':
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						case '9':
						{
							current.type = NUMBER;
							break;
						}
						default: std::cerr << "Unknown token: " << current.raw[0] << std::endl; break;
					};
				}
			}
			else if(current.raw.size() > 1){
				if(current.raw == "=="){
					current.type = EQ;
				}
				else if(current.raw == ">="){
					current.type = GEQ;
				}
				else if(current.raw == "<="){
					current.type = LEQ;
				}
				else{
					current.type = NUMBER;
					for(int i = 0; i < current.raw.size(); i++){
						if(i == 0 && current.raw[0] == '-'){ continue; }
						if(!isdigit(current.raw[i])){
							current.type = IDENTIFIER;
						}
					}
					if(current.type == IDENTIFIER && current.raw[current.raw.size()-1] == ':'){
						current.type = LABEL;
					}
					else if(current.type == IDENTIFIER && current.raw[0] == '"' && current.raw[current.raw.size()-1] == '"'){
						current.type = STRING;
					}
				}
			}
			tokens.push_back(current);
			if(buffer[i] == '\n'){
				current.type = NEWLINE;
				current.raw = '\n';
				tokens.push_back(current);
			}
			current.raw.clear();
		}
		else{
			current.raw.push_back(buffer[i]);
		}
	}

	if(inString){
		std::cerr << "Unbounded string found" << std::endl;
		success = false;
	}
}
		
Parser::Parser(std::vector<Token> tokens){
	Statement current;

	for(int i = 0; i < tokens.size(); i++){
		switch(tokens[i].type){
			case LABEL:
			{
				current.type = LABEL_IDENTIFIER;
				current.tokens.push_back(tokens[i]);
				statements.push_back(current);
				current.tokens.clear();
				break;
			}
			case ADD:
			case SUBTRACT:
			case MULTIPLY:
			case DIVIDE:
			{
				if(i-1 < 0 || i+1 >= tokens.size()){
					std::cerr << "Improper math statement found" << std::endl;
					success = false;
					break;
				}
				current.type = MATH;
				current.tokens.push_back(tokens[i-1]);
				current.tokens.push_back(tokens[i]);
				current.tokens.push_back(tokens[i+1]);
				statements.push_back(current);
				current.tokens.clear();
				i++;
				break;
			}
			case ASSIGN:
			{
				if(i-1 < 0 || i+1 >= tokens.size()){
					std::cerr << "Improper math statement found" << std::endl;
					success = false;
					break;
				}
				if(i-2 >= 0 && tokens[i-2].type == IDENTIFIER){
					current.type = VAR_INIT;
					current.tokens.push_back(tokens[i-2]);
				}
				else{
					current.type = VAR_ASSIGN;
				}
				current.tokens.push_back(tokens[i-1]);
				current.tokens.push_back(tokens[i]);
				statements.push_back(current);
				current.tokens.clear();
				break;
			}
			case NUMBER:
			case STRING:
			{
				current.type = CONSTANT;
				current.tokens.push_back(tokens[i]);
				statements.push_back(current);
				current.tokens.clear();
				break;
			}
			case LT:
			case LEQ:
			case GT:
			case GEQ:
			case EQ:
			{
				if(i-1 < 0 || i+1 >= tokens.size()){
					std::cerr << "Improper comparison statement found" << std::endl;
					success = false;
					break;
				}
				current.type = COMPARISON;
				current.tokens.push_back(tokens[i-1]);
				current.tokens.push_back(tokens[i]);
				current.tokens.push_back(tokens[i+1]);
				statements.push_back(current);
				current.tokens.clear();
				i++;
				break;
			}
			case IDENTIFIER:
			{
				if(tokens[i].raw == "print"){
					if(i+1 >= tokens.size()){
						std::cerr << "Improper print statement found" << std::endl;
						success = false;
						break;
					}
					current.type = PRINT;
					current.tokens.push_back(tokens[i]);
					current.tokens.push_back(tokens[i+1]);
					statements.push_back(current);
					current.tokens.clear();
				}
				else if(tokens[i].raw == "goto"){
					if(i+1 >= tokens.size()){
						std::cerr << "Improper goto statement found" << std::endl;
						success = false;
						break;
					}
					current.type = GOTO;
					current.tokens.push_back(tokens[i]);
					current.tokens.push_back(tokens[i+1]);
					statements.push_back(current);
					current.tokens.clear();
				}
				else if(tokens[i].raw == "if"){
					current.type = IF;
					current.tokens.push_back(tokens[i]);
					statements.push_back(current);
					current.tokens.clear();
				}
				else if(tokens[i].raw == "return"){
					current.type = RETURN;
					current.tokens.push_back(tokens[i]);
					statements.push_back(current);
					current.tokens.clear();
				}
			}
			default: break;
		};
	}
}

Interpreter::Interpreter(std::vector<Statement> statements){
	for(int i = 0; i < statements.size(); i++){
		switch(statements[i].type){
			case VAR_INIT:
			{
				Variable var;
				var.type = statements[i].tokens[0].raw == "int" ? INT : STR;
varset:
				if(statements[i+1].type == CONSTANT){
					if(var.type == INT){
						var.number = std::stoi(statements[i+1].tokens[0].raw);
					}
					else if(var.type == STR){
						var.string = statements[i+1].tokens[0].raw;
					}
					i++;
				}
				else if(statements[i+1].type == COMPARISON || statements[i+1].type == MATH){
					statements[i+1] = simplify(statements[i+1]);
					goto varset;
				}

				if(vars.contains(statements[i-1].tokens[1].raw)){
					std::cerr << "Attempt to redeclare variable \"" << statements[i].tokens[1].raw << "\"" << std::endl;
					exit(1);
				}
				vars[statements[i-1].tokens[1].raw] = var;
				break;
			}
			case VAR_ASSIGN:
			{
				if(!vars.contains(statements[i].tokens[0].raw)){
					std::cerr << "Bad attempt to assign to variable \"" << statements[i].tokens[1].raw << "\"" << std::endl;
					exit(1);
				}

				Statement assigning = statements[i+1];

varassign:
				if(assigning.type == CONSTANT){
					if(vars[statements[i].tokens[0].raw].type == INT){
						vars[statements[i].tokens[0].raw].number = std::stoi(assigning.tokens[0].raw);
					}
					else if(vars[statements[i].tokens[0].raw].type == STR){
						vars[statements[i].tokens[0].raw].string = assigning.tokens[0].raw;
					}
					i++;
				}
				else if(assigning.type == COMPARISON || assigning.type == MATH){
					assigning = simplify(assigning);
					goto varassign;
				}
				break;
			}
			case MATH:
			case COMPARISON:
			{
				statements[i] = simplify(statements[i]);
				break;
			}
			case LABEL_IDENTIFIER:
			{
				if(labels.contains(statements[i].tokens[0].raw)){
					std::cerr << "Redeclaration of label \"" << statements[i].tokens[0].raw << "\"" << std::endl;
					exit(1);
				}
				statements[i].tokens[0].raw.pop_back();
				labels[statements[i].tokens[0].raw] = i;
				break;
			}
			case PRINT:
			{
				if(statements[i].tokens[1].type == IDENTIFIER){
					if(!vars.contains(statements[i].tokens[1].raw)){
						std::cerr << "Attempt to print nonexistent variable" << std::endl;
						exit(1);
					}
					Variable var = vars[statements[i].tokens[1].raw];
					if(var.type == INT){
						std::cout << "[Program Output] " << var.number << std::endl;
					}
					else if(var.type == STR){
						std::cout << "[Program Output] " << var.string << std::endl;
					}
				}
				else{
					// assume its a constant
					std::cout << "[Program Output] " << statements[i].tokens[1].raw << std::endl;
				}
				break;
			}
			case GOTO:
			{
				if(labels.contains(statements[i].tokens[1].raw)){
					i = labels[statements[i].tokens[1].raw];
				}
				else{
					std::cerr << "Attempt to goto nonexistent label" << std::endl;
					exit(1);
				}
				break;
			}
			case IF:
			{
				if(statements[i+1].type == COMPARISON){
					Statement ifResult = simplify(statements[i+1]);
					if(ifResult.tokens[0].raw == "0"){
						i += 2; // skip 2 to skip the if condition
					}
				}
				else{
					std::cerr << "Attempt to if without comparison in next statement" << std::endl;
					exit(1);
				}
				break;
			}
			case RETURN:
			{
				return;
			}
			default: break;
		}
	}
}

Statement Interpreter::simplify(Statement statement){
	Statement res;
	res.type = CONSTANT;
	Token token;
	token.type = NUMBER;
	switch(statement.type){
		case MATH:
		{
			int lhs, rhs;
			if(statement.tokens[0].type == IDENTIFIER){
				if(vars.contains(statement.tokens[0].raw) && vars[statement.tokens[0].raw].type == INT){
					lhs = vars[statement.tokens[0].raw].number;
				}
				else{
					std::cerr << "Attempted to do math with bad identifier" << std::endl;
					exit(1);
				}
			}
			else if(statement.tokens[0].type == NUMBER){
				lhs = std::stoi(statement.tokens[0].raw);
			}
			else{
				std::cerr << "Attempted to do math with bad identifier" << std::endl;
				exit(1);
			}
			if(statement.tokens[2].type == IDENTIFIER){
				if(vars.contains(statement.tokens[2].raw) && vars[statement.tokens[2].raw].type == INT){
					rhs = vars[statement.tokens[2].raw].number;
				}
				else{
					std::cerr << "Attempted to do math with bad identifier" << std::endl;
					exit(2);
				}
			}
			else if(statement.tokens[2].type == NUMBER){
				rhs = std::stoi(statement.tokens[2].raw);
			}
			else{
				std::cerr << "Attempted to do math with bad identifier" << std::endl;
				exit(1);
			}

			switch(statement.tokens[1].type){
				case ADD: token.raw = std::to_string(lhs+rhs); break;
				case SUBTRACT: token.raw = std::to_string(lhs-rhs); break;
				case MULTIPLY: token.raw = std::to_string(lhs*rhs); break;
				case DIVIDE: token.raw = std::to_string(lhs/rhs); break;
				default: break;
			};
		}
		case COMPARISON:
		{
			int lhs, rhs;
			if(statement.tokens[0].type == IDENTIFIER){
				if(vars.contains(statement.tokens[0].raw) && vars[statement.tokens[0].raw].type == INT){
					lhs = vars[statement.tokens[0].raw].number;
				}
				else{
					std::cerr << "Attempted to do math with bad identifier" << std::endl;
					exit(1);
				}
			}
			else if(statement.tokens[0].type == NUMBER){
				lhs = std::stoi(statement.tokens[0].raw);
			}
			else{
				std::cerr << "Attempted to do math with bad identifier" << std::endl;
				exit(1);
			}
			if(statement.tokens[2].type == IDENTIFIER){
				if(vars.contains(statement.tokens[2].raw) && vars[statement.tokens[2].raw].type == INT){
					rhs = vars[statement.tokens[2].raw].number;
				}
				else{
					std::cerr << "Attempted to do math with bad identifier" << std::endl;
					exit(2);
				}
			}
			else if(statement.tokens[2].type == NUMBER){
				rhs = std::stoi(statement.tokens[2].raw);
			}
			else{
				std::cerr << "Attempted to do math with bad identifier" << std::endl;
				exit(1);
			}

			switch(statement.tokens[1].type){
				case LT: token.raw = (lhs < rhs ? "1" : "0"); break;
				case LEQ: token.raw = (lhs <= rhs ? "1" : "0"); break;
				case GT: token.raw = (lhs > rhs ? "1" : "0"); break;
				case GEQ: token.raw = (lhs >= rhs ? "1" : "0"); break;
				case EQ: token.raw = (lhs == rhs ? "1" : "0"); break;
				default: break;
			}
		}
		default: break;
	}

	if(token.raw.empty()){
		std::cerr << "Bad simplify" << std::endl;
		exit(1);
	}
	res.tokens.push_back(token);
	return res;
}
