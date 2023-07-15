#include "Parser.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main() {
    typedef std::vector<std::pair<int, std::string> >   Type;

	std::ifstream file("test.conf");

	Parser<Type> parser;
	Type values = parser.parse(file);
	for (Type::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
		std::cout << iter->first << iter->second << std::endl;
	}
}