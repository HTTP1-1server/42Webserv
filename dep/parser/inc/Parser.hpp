#pragma once

#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include "HashMap.hpp"

typedef void (FormatFunc)(std::istream &, const std::string *key, HashMap &);
typedef std::map<std::string, FormatFunc *>	Formatters;

class Parser {
public:
	Parser(): blockEnd("}") {};
	virtual ~Parser() {};

	Formatters fomatters;
	HashMap tags;

	std::string blockEnd;

	void setFormat(std::string key, FormatFunc *formatFunc);
	HashMap parse(std::istream & stream);

	std::string extractKey(std::istream &stream);
	static void skipLineEnd(std::istream &stream);

	static void parseIntTag(std::istream &stream, const std::string *key, HashMap &tags);
	static void parseStringTag(std::istream &stream, const std::string *key, HashMap &tags);
	static void parseVecOfStringTag(std::istream &stream, const std::string *key, HashMap &tags);
};