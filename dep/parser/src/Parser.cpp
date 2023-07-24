#include "Parser.hpp"

void Parser::setFormat(std::string key, FormatFunc *formatFunc) {
	std::pair<Formatters::iterator, bool> insertion = fomatters.insert(std::make_pair(key, formatFunc));
	bool isKeyDuplicated = !insertion.second;
	if (isKeyDuplicated) {
		throw std::runtime_error("Tried to set a format with a duplicated key");
	}
}

HashMap Parser::parse(std::istream & stream) {
	while (stream.good()) {
		std::string key = this->extractKey(stream);
		if (key == blockEnd || key.empty()) {
			break;
		}
		FormatFunc *formatFunc = fomatters.at(key);
		formatFunc(stream, &key, this->tags);
	}
	return this->tags;
};

std::string Parser::extractKey(std::istream &stream) {
	std::string key;
	stream >> key;
	// std::cout << "KEY: " << key << key.length() << std::endl;
	if (stream.fail() && !stream.eof()) {
		stream.clear();
		throw std::runtime_error("parse key error");
	}
	return key;
}

void Parser::skipLineEnd(std::istream &stream) {
	if (stream.good()) {
		std::string line;
		std::getline(stream, line);
	}
}

void Parser::parseIntTag(std::istream &stream, const std::string *key, HashMap &tags) {
	int port;
	stream >> port;
	if (stream.fail()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	Parser::skipLineEnd(stream);

	tags.insert(std::make_pair(*key, UniquePtr<Any>(AnyType<int>(port))));
}

void Parser::parseStringTag(std::istream &stream, const std::string *key, HashMap &tags) {
	std::string host;
	stream >> host;
	if (stream.fail() || host.empty()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	Parser::skipLineEnd(stream);
	if (*host.rbegin() == ';')
		host.resize(host.length() - 1);

	tags.insert(std::make_pair(*key, UniquePtr<Any>(AnyType<std::string>(host))));
}

void Parser::parseVecOfStringTag(std::istream &stream, const std::string *key, HashMap &tags) {
	std::vector<std::string> methods;
	std::string line;
	std::getline(stream, line);
	if (stream.fail()) {
		stream.clear();
		throw std::runtime_error("parse error: " + *key);
	}
	std::istringstream iss(line);
	while (iss.good()) {
		std::string method;
		iss >> method;
		if (iss.fail() || method.empty()) {
			throw std::runtime_error("parse error: " + *key);
		}
		if (*method.rbegin() == ';')
			method.resize(method.length() - 1);
		if (method.empty())
			throw std::runtime_error("parse error: " + *key);
		methods.push_back(method);
	}
	if (methods.empty())
		throw std::runtime_error("parse error: " + *key);

	tags.insert(std::make_pair(*key, UniquePtr<Any>(AnyType<std::vector<std::string> >(methods))));
}