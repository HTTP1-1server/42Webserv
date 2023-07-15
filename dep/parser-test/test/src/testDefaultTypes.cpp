#include "Parser.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main() {
    
	std::ifstream file("test.conf");
	// Parser<Type::Integer>	intParser;
	// Parser<Type::String>	stringParser;
	// while (!file.eof()) {
	// 	try {
	// 		std::cout << intParser.parse(file);
	// 	}
	// 	catch(const std::exception& e)
	// 	{
	// 		std::cout << stringParser.parse(file) << std::endl;
	// 	}
	// }

	Parser<std::vector<int> > intListParser;
	std::vector<int> intList = intListParser.parse(file);
	for (std::vector<int>::const_iterator iter = intList.begin(); iter != intList.end(); ++iter) {
		std::cout << *iter << std::endl;
	}
}