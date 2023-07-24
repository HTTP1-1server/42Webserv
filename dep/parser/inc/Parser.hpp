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
typedef std::map<std::string, FormatFunc *>	Formatters;

class Parser {
public:
	Parser(): blockEnd("}") {};
	virtual ~Parser() {};

	Formatters fomatters;
	Tags tags;

	std::string blockEnd;

	void setFormat(std::string key, FormatFunc *formatFunc);
	Tags parse(std::istream & stream);

	std::string extractKey(std::istream &stream);
	static void skipLineEnd(std::istream &stream);

	static void parseIntTag(std::istream &stream, const std::string *key, Tags &tags);
	static void parseStringTag(std::istream &stream, const std::string *key, Tags &tags);
	static void parseVecOfStringTag(std::istream &stream, const std::string *key, Tags &tags);
};