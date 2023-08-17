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

std::map<std::string, std::string> Parser::parseHttpHeader(std::istream &stream) {
	std::map<std::string, std::string> header;

	while (stream.good()) {
		std::string key = this->extractKey(stream);
		if (key == "\r" || key.empty() || stream.fail()) {
			break;
		}
		if (*key.rbegin() == ':')
			key.pop_back();
		std::string value = this->extractKey(stream);
		if (value == "\r" || value.empty()|| stream.fail()) {
			break;
		}
		header.insert(std::make_pair(key, value));
	}
	return header;
}

std::string Parser::parseChunkedBody(std::string &stream) {
	// \r\n\r\n1\r\n0\r\n0\r\n\r\n
	// \r\n1\r\n0\r\n0\r\n\r\n
	// ffff\r\n0\r\n0\r\n\r\n
	// \r\n0\r\n0\r\n\r\n
	// 0 1 2 3 45 6 78 9 1011121314
	std::string body;

	while (!stream.empty() && stream != "0\r\n\r\n") {
		int pos = stream.find("\r\n");
		if (pos == 0) {
			stream.erase(0, 2);
			continue;
		}
		std::stringstream ss;
		ss << std::hex << stream;
		// ss = ffff\r\n0\r\n0\r\n\r\n
		unsigned int chunkLength;
		ss >> chunkLength;

		int hexChunkedLength = stream.length() - ss.str().length();
	
		// F\r\n0\r\n0\r\n\r\n
		// 16
		// 1 2  1 2 
		// 0\r\n\r\n
		// 01 2 34 5
		int startPos = stream.find("\r\n") + 2; //3
		int endPos = stream.find("\r\n", startPos); //4
		std::string chunkLiteral = stream.substr(startPos, endPos - startPos);
		body.append(chunkLiteral);
		stream = stream.substr(endPos + 2, stream.length() - hexChunkedLength - 2 - chunkLiteral.length() - 2);
	}
	return body;
}