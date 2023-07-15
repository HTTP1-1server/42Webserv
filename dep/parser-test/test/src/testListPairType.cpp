#include "Parser.hpp"

#include <fstream>
#include <iostream>
#include <vector>

int main() {
    typedef std::vector<std::pair<std::vector<int>, std::string> >   Type;

	std::ifstream file("testListPair.conf");

	Parser<Type> parser;
	Type values = parser.parse(file);
	for (Type::const_iterator iter = values.begin(); iter != values.end(); ++iter) {
		for (std::vector<int>::const_iterator innerIter = iter->first.begin(); innerIter != iter->first.end(); ++innerIter) {
			std::cout << *innerIter << " ";
		}
		std::cout << iter->second << std::endl;
	}
}