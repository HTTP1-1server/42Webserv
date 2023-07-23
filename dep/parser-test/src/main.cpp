#include "Parser.hpp"
#include "ServerConfig.hpp"

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

int main() {
	Parser parser;
	parser.setFormat("server", &ServerConfig::parseServerBlock);
	
	std::ifstream file("default.conf");
	ServerConfig serverList = (ServerConfig)parser.parse(file);

	std::vector<ServerConfig> ServerConfigs = *serverList.at("server").data;
	for (std::vector<ServerConfig>::const_iterator serverConfig = ServerConfigs.begin(); serverConfig != ServerConfigs.end(); ++serverConfig) {
		int port = *serverConfig->at("listen").data;
		std::string host = *serverConfig->at("host").data;


		HashMap locationMap = *serverConfig->at("location").data;
		KeyList locations = locationMap.getKeys();
		HashMap pathLoc = *locationMap.at("/path").data;
		HashMap path2Loc = *locationMap.at("/path2").data;


		std::cout << "PORT: " << port << std::endl;
		std::cout << "HOST: " << host << std::endl;
		std::cout << "LOC: " << locations.size() << std::endl;
		for (HashMap::const_iterator loc = locationMap.begin(); loc != locationMap.end(); ++loc) {
			std::cout << "\tPATH: " << loc->first << std::endl;
			HashMap path = *loc->second.data;
			for (HashMap::const_iterator subTag = path.begin(); subTag != path.end(); ++subTag) {
				std::vector<std::string> value = *subTag->second.data;
				std::cout << "\t\tSUBKEY: " << subTag->first;
				for (std::vector<std::string>::const_iterator method = value.begin(); method != value.end(); ++method)
					std::cout << " " << *method;
				std::cout << std::endl;
			}
		}
	}
}