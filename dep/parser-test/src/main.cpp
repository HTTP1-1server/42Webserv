#include "Parser.hpp"
#include "ServerConfig.hpp"

#include <map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>

// typedef std::multimap<std::string, Data> ServerConfig;

// template <typename ServerConfigs>
// void parse(ServerConfigs serverConfigs) {
//     for(typename ServerConfigs::iterator serverConfig = serverConfigs.begin();
//         serverConfig != serverConfigs.end();
//         ++serverConfig) {
//         //std::cout << serverConfig << std::endl;
//     }
// }

int main() {
	Parser<LocationConfig> locationParser;
	locationParser.setBlock("location", Parser::String);
	locationParser.setTag("allow_method", Parser::List<std::string>);

	Parser<ServerConfig> parser;
	parser.setBlock("server");
	parser.setTag("listen", Parser::Integer);
	parser.setTag("host", Parser::List<std::string>);
	parser.setTag("error_page", (Parser::List<int>, Parser::String));
	parser.setTag(locationParser);
	parser.setCommentPrefix("#");
	parser.setLineEndSuffix(";");
	parser.setBlockBracket("{", "}");
	parser.setDelemiters({" ", "\t"});

	std::string filename("default.conf");
	std::ifstream file(filename);
	ServerConfig serverConfig = parser.parse(file);
}