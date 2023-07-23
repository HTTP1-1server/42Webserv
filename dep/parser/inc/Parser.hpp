#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "HashMap.hpp"

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
		// std::cout << "KEY: " << key << key.length() << std::endl;
		if (stream.fail() && !stream.eof()) {
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

	static void parseIntTag(std::istream &stream, const std::string *key, Tags &tags) {
		int port;
        stream >> port;
        if (stream.fail()) {
            stream.clear();
            throw std::runtime_error("parse error: " + *key);
        }
        Parser::skipLineEnd(stream);

		tags.insert(std::make_pair(*key, UniquePtr<Any>(AnyType<int>(port))));
	}

	static void parseStringTag(std::istream &stream, const std::string *key, Tags &tags) {
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
	
	static void parseVecOfStringTag(std::istream &stream, const std::string *key, Tags &tags) {
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
};

// #include "Parser.tpp"