#include "interpreter.hpp"
#include <iostream>

int main(int argc, char** argv){
	if(argc < 2){
		std::cerr << "Not given file to interpret" << std::endl;
		return 1;
	}

	bool debug = false;
	if(argc > 2){
		std::string s = argv[2];
		if(s == "debug"){
			debug = true;
		}
	}

	Lexer lexer(argv[1]);
	Parser parser(lexer.getTokens());

	if(debug){
		for(int i = 0; i < parser.getStatements().size(); i++){
			Statement statement = parser.getStatements()[i];
			std::cout << "----------\nStatements\nType: " 
								<< statement.type 
								<< "\nTokens: [\n";
			for(int j = 0; j < statement.tokens.size(); j++){
				std::cout << "\tType: " << statement.tokens[j].type
									<< ", Raw: \"" << statement.tokens[j].raw << "\"\n";
			}
			std::cout << "]" << std::endl;
		}
		std::cout << "-----" << std::endl;
	}

	Interpreter interpreter(parser.getStatements());

	return 0;
}
