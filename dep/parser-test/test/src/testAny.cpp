#include "Any.hpp"

#include <map>
#include <string>
#include <iostream>
#include <vector>

int main() {
	Dictionary anys;
	anys.insert(std::make_pair("listen", new AnyType<int>(42)));

	std::vector<std::string> hosts;
	hosts.push_back("such a ridiculous");
	hosts.push_back("holy moly");
	anys.insert(std::make_pair("host", new AnyType<std::vector<std::string> >(hosts)));

	int port = *anys.at("listen");
	std::cout << "PORT: " << port << std::endl;

	std::vector<std::string> &hostsFromAnys = *anys.at("host");
	hostsFromAnys.push_back("is it true?");

	std::vector<std::string> hostsFromAnysMutated = *anys.at("host");
	for (std::vector<std::string>::const_iterator iter = hostsFromAnysMutated.begin(); iter != hostsFromAnysMutated.end(); ++iter) {
		std::cout << "HOST: " << *iter << std::endl;
	}

	Any *absentAny = anys["NULL"];
	if (absentAny == NULL)
		std::cout << "NULL: (null)" << std::endl;

	Any *absentAnyAsserted = anys.at("ABNORMAL");
	if (absentAnyAsserted == NULL)
		std::cout << "ABNORMAL: (abnormal)" << std::endl;
}