#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "Any.hpp"

typedef HashMap Tags;
typedef void (FormatFunc)(std::istream &, const std::string *key, Tags &);
typedef std::map<std::string, const FormatFunc *>	Formatters;

class Parser {
public:
	Parser(): blockEnd("}") {};
	virtual ~Parser() {};

	Formatters fomatters;
	Tags tags;

	std::string blockEnd;

	void setFormat(std::string key, FormatFunc *formatFunc) {
		std::pair<Formatters::iterator, bool> insertion = fomatters.insert(std::make_pair(key, formatFunc));
		bool isKeyDuplicated = !insertion.second;
		if (isKeyDuplicated) {
			throw std::runtime_error("Tried to set a format with a duplicated key");
		}
	}

	Tags parse(std::istream & stream) {
		while (stream.good()) {
			std::string key = this->extractKey(stream);
			// std::cout << "KEY: " << key << std::endl;
			if (key == blockEnd || key.empty()) {
				break;
			}
			const FormatFunc *formatFunc = fomatters.at(key);
			formatFunc(stream, &key, this->tags);
		}
		return this->tags;
	};

	std::string extractKey(std::istream &stream) {
		std::string key;
		stream >> key;
		if (stream.fail()) {
			stream.clear();
			throw std::runtime_error("parse key error");
		}
		return key;
	}

	static void skipLineEnd(std::istream &stream) {
		if (stream.good()) {
			std::string line;
			std::getline(stream, line);
		}
	}
};

// #include "Parser.tpp"