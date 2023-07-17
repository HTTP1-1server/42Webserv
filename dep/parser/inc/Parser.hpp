#pragma once

#include <map>
#include <sstream>
#include <fstream>
#include <vector>

enum Namability {
	Titled,
	Untitled,
};

template <class T, Namability N = Untitled>
class Parser {
public:
	Parser() {};
	~Parser() {};

	std::multimap<std::string, Parser> hm;

	T parse(std::istream & stream) const { return T();};

	// template <class Value>
	// void setNamedValue(std::string name) {
	// 	hm.insert(std::make_pair(name, Value()));
	// }
};

template <>
class Parser<int> {
public:
	Parser() {};
	~Parser() {};

	int parse(std::istream &stream) const {
		int value;
		stream >> value;
		if (stream.fail()) {
			stream.clear();
			throw std::runtime_error("Parse<int> error");
		}
		return value;
	};
};

template <>
class Parser<std::string> {
public:
	Parser() {};
	~Parser() {};

	std::string parse(std::istream &stream) const {
		std::string value;
		stream >> value;
		if (stream.fail()) {
			stream.clear();
			throw std::runtime_error("Parse<std::string> error");
		}
		return value;
	};
};

template <class T>
class Parser<std::vector<T> > {
public:
	Parser() {};
	~Parser() {};

	std::vector<T> parse(std::istream &stream) const {
		std::vector<T>	values;
		while (stream.good()) {
			try {
				T value = Parser<T>().parse(stream);
				values.push_back(value);
			} catch (std::exception &e) {
				stream.clear();
				if (values.size() == 0)
					throw std::runtime_error("Parse<std::vector> error");
				return values;
			}
		}
		return values;
	};
};

template <class T1, class T2>
class Parser<std::pair<T1, T2> > {
public:
	Parser() {};
	~Parser() {};

	std::pair<T1, T2> parse(std::istream &stream) const {
		T1 value1 = Parser<T1>().parse(stream);
		T2 value2 = Parser<T2>().parse(stream);
		return std::make_pair(value1, value2);
	};
};


// #include "Parser.tpp"