#include <vector>
#include <stdexcept>
#include "Server.hpp"
#include "ServletApplication.hpp"

#define DEFAULT_FILENAME ("default.conf")

std::vector<ServerConfig> parseConfiguration(const std::string &filename);

int main(int argc , char *argv[])  
{
	if (argv == NULL || argc < 1)
		throw std::runtime_error("arguments error");

	std::string filename = argv[1] == NULL ? std::string(DEFAULT_FILENAME) : std::string(argv[1]);
	std::vector<ServerConfig> serverConfigs = parseConfiguration(filename);
	
    ServletApplication servletApplication(serverConfigs);
    servletApplication.run();
    return 0;
}

std::vector<ServerConfig> parseConfiguration(const std::string &filename) {
    Parser parser;
	parser.setFormat("server", &ServerConfig::parseServerBlock);
	
	std::ifstream file(filename.c_str());
	HashMap serverList = (HashMap)parser.parse(file);

	std::vector<ServerConfig> serverConfigs = *serverList.at("server").data;
	return serverConfigs;
}