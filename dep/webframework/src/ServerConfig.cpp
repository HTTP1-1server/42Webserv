#include "ServerConfig.hpp"

void ServerConfig::parseLocationTag(std::istream &stream, const std::string *key, HashMap &tags) {
	std::string path;
	stream >> path;
	if (stream.fail() || path.empty()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	Parser::skipLineEnd(stream);

	UniquePtr<Any> locationGroup = UniquePtr<Any>(AnyType<HashMap>(HashMap()));
	std::pair<HashMap::iterator, bool> insertion = tags.insert(std::make_pair(*key, locationGroup));

	HashMap &locations = (HashMap &)*insertion.first->second.data;
	
	Parser locationParser;
	locationParser.setFormat("allowed_methods", &Parser::parseVecOfStringTag);

	locations.insert(std::make_pair(path, UniquePtr<Any>(AnyType<HashMap>(locationParser.parse(stream)))));
}

void ServerConfig::parseErrorTag(std::istream &stream, const std::string *key, HashMap &tags) {
	std::vector<std::string> errorCodeAndPath;
	std::string line;
	std::getline(stream, line);
	if (stream.fail()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	std::istringstream iss(line);
	while (iss.good()) {
		std::string error;
		iss >> error;
		if (iss.fail() || error.empty()) {
			throw std::runtime_error("parse error: " + *key);
		}
		if (*error.rbegin() == ';')
			error.resize(error.length() - 1);
		if (error.empty())
			throw std::runtime_error("parse error: " + *key);
		errorCodeAndPath.push_back(error);
	}
	if (errorCodeAndPath.size() < 2)
		throw std::runtime_error("parse error: " + *key);

	UniquePtr<Any> errorGroup = UniquePtr<Any>(AnyType<std::map<int, std::string> >(std::map<int, std::string>()));
	std::pair<HashMap::iterator, bool> insertion = tags.insert(std::make_pair(*key, errorGroup));

	std::map<int, std::string> &errors = (std::map<int, std::string> &)*insertion.first->second.data;

	std::string errorPage = *errorCodeAndPath.rbegin();
	for (size_t i = 0; i < errorCodeAndPath.size() - 1; ++i) {
		int code;
		std::istringstream iss(errorCodeAndPath[i]);
		iss >> code;
		errors.insert(std::make_pair(code, errorPage));
	}
}

ServerConfig::ServerConfig(const HashMap &other): HashMap(other) {};

void ServerConfig::parseServerBlock(std::istream &stream, const std::string *key, HashMap &tags) {
	std::string server;
	stream >> server;
	if (stream.fail() || server.empty()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	Parser::skipLineEnd(stream);

	UniquePtr<Any> serverGroup = UniquePtr<Any>(AnyType<std::vector<ServerConfig> >(std::vector<ServerConfig>()));
	std::pair<HashMap::iterator, bool> insertion = tags.insert(std::make_pair(*key, serverGroup));

	std::vector<ServerConfig> &servers = (std::vector<ServerConfig> &)*insertion.first->second.data;

	Parser serverParser;
	serverParser.setFormat("listen", &Parser::parseIntTag);
	serverParser.setFormat("host", &Parser::parseStringTag);
	serverParser.setFormat("location", &ServerConfig::parseLocationTag);
	serverParser.setFormat("error_page", &ServerConfig::parseErrorTag);
	serverParser.setFormat("index", &Parser::parseStringTag);

	servers.push_back(serverParser.parse(stream));
}