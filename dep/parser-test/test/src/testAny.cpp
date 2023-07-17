#include "Any.hpp"

#include <map>
#include <string>
#include <iostream>
#include <vector>

int main() {
	Dictionary anys;
	anys.insert(std::make_pair("listen", new Parameter<int>(42)));
	anys.insert(std::make_pair("host", new Parameter<std::string>("such a ridiculous")));
	anys.insert(std::make_pair("host", new Parameter<std::string>("holy moly")));
	Dictionary::pair_iterator range = anys.equal_range("listen");
	int port = (int)*anys["kk"];
	std::cout << "PORT: " << port << std::endl;
	for (Dictionary::const_iterator iter = range.first; iter != range.second; ++iter) {
		std::cout << iter->first << ": " << (int)*iter->second << std::endl;
	}
	Dictionary::pair_iterator range2 = anys.equal_range("host");
	for (Dictionary::const_iterator iter = range2.first; iter != range2.second; ++iter) {
		std::cout << iter->first << ": " << (std::string)*iter->second << std::endl;
	}

	// Dictionary subAnys;
	// subAnys.insert(std::make_pair("/path1", new Parameter<std::string>("sub value1")));
	// subAnys.insert(std::make_pair("/path1", new Parameter<std::string>("sub value2")));

	// anys.insert(std::make_pair("location", new Parameter<Dictionary>(subAnys)));
	// anys.insert(std::make_pair("location", new Parameter<Dictionary>(subAnys)));
	// Dictionary::pair_iterator range3 = anys.equal_range("location");
	// for (Dictionary::const_iterator iter = range3.first; iter != range3.second; ++iter) {
	// 	std::cout << iter->first << ": " << std::endl;
	// 	Dictionary &subDictionary = *iter->second;
	// 	Dictionary::pair_iterator subRange = subDictionary.equal_range("/path1");
	// 	for (Dictionary::const_iterator iter2 = subRange.first; iter2 != subRange.second; ++iter2) {
	// 		std::cout << "\t" << iter2->first << ": " << (std::string)*iter2->second << std::endl;
	// 	}
	// }

}