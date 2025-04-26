#include "interpreter.hpp"
#include <iostream>

int main(int argc, char** argv){
	if(argc < 2){
		std::cerr << "Not given file to interpret" << std::endl;
		return 1;
	}

	Lexer lexer(argv[1]);
	Parser parser(lexer.getTokens());
	Interpreter interpreter(parser.getStatements());

	return 0;
}
