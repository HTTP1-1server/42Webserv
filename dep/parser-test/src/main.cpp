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
	parser.setFormat("listen", &ServerConfig::parseListenTag);
	parser.setFormat("host", &ServerConfig::parseHostTag);
	parser.setFormat("location", &ServerConfig::parseLocationTag);
	
	std::ifstream file("default.conf");
	ServerConfig serverConfig = (ServerConfig)parser.parse(file);

	int port = *serverConfig.at("listen");
	std::string host = *serverConfig.at("host");

	Dictionary locations = *serverConfig.at("location");
	Dictionary pathLoc = *locations.at("/path");

	std::cout << "PORT: " << port << std::endl;
	std::cout << "HOST: " << host << std::endl;
	for (Dictionary::const_iterator loc = locations.begin(); loc != locations.end(); ++loc) {
		std::cout << "PATH: " << loc->first << std::endl;
		Dictionary path = *loc->second;
		for (Dictionary::const_iterator subTag = path.begin(); subTag != path.end(); ++subTag) {
			std::vector<std::string> value = *subTag->second;
			std::cout << "SUBKEY: " << subTag->first;
			for (std::vector<std::string>::const_iterator method = value.begin(); method != value.end(); ++method)
			 	std::cout << " " << *method;
			std::cout << std::endl;
		}
	}
}