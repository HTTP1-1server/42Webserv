#include "Parser.hpp"
#include "ServerConfig.hpp"

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

// std::map<std::string, Any *>

int main() {
	Parser parser;
	parser.setFormat("listen", &Parser::parseIntTag);
	parser.setFormat("host", &Parser::parseStringTag);
	parser.setFormat("location", &ServerConfig::parseLocationTag);
	
	std::ifstream file("default.conf");
	ServerConfig serverConfig = (ServerConfig)parser.parse(file);

	int port = *serverConfig.at("listen");
	std::string host = *serverConfig.at("host");

	HashMap locationMap = *serverConfig.at("location");
	KeyList locations = locationMap.getKeys();
	HashMap pathLoc = *locationMap.at("/path");
	HashMap path2Loc = *locationMap.at("/path2");


	std::cout << "PORT: " << port << std::endl;
	std::cout << "HOST: " << host << std::endl;
	std::cout << "LOC: " << locations.size() << std::endl;
	for (HashMap::const_iterator loc = locationMap.begin(); loc != locationMap.end(); ++loc) {
		std::cout << "\tPATH: " << loc->first << std::endl;
		HashMap path = *loc->second;
		for (HashMap::const_iterator subTag = path.begin(); subTag != path.end(); ++subTag) {
			std::vector<std::string> value = *subTag->second;
			std::cout << "\t\tSUBKEY: " << subTag->first;
			for (std::vector<std::string>::const_iterator method = value.begin(); method != value.end(); ++method)
			 	std::cout << " " << *method;
			std::cout << std::endl;
		}
	}
}