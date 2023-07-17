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
	locationParser.setTitledBlock<Parser::String>("location");
	locationParser.setTag<Parser::List<std::string>>("allow_method");

	Parser<ServerConfig> parser;
	parser.setUntitledBlock("server");
	parser.setTag<Parser::Integer>("listen");
	parser.setTag<Parser::List<std::string>>("host");
	parser.setTag<Parser::List<std::string>>("seongspa");
	// parser.setTag<(Parser::List<int>, Parser::String)>("error_page", ("code", "path"));
	parser.setTag(locationParser);
	parser.setCommentPrefix("#");
	parser.setLineEndSuffix(";");
	parser.setBlockBracket("{", "}");
	parser.setDelemiters({" ", "\t"});

	std::string filename("default.conf");
	std::ifstream file(filename);
	ServerConfig serverConfig = parser.parse(file);

	int listen = serverConfig["listen"];
	std::vector<std::string> host = serverConfig["host"];

}