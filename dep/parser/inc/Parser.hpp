#pragma once

#include <map>
#include <sstream>
#include <fstream>
#include <vector>

namespace Type {
	typedef int				Integer;
	typedef std::string		String;
}

template <class T>
class Parser {
public:
	Parser();
	~Parser();

	T parse(std::istream & stream) const;
};

template <>
class Parser<Type::Integer> {
public:
	Parser() {};
	~Parser() {};

	Type::Integer parse(std::istream &stream) const {
		Type::Integer value;
		stream >> value;
		if (stream.fail()) {
			stream.clear();
			throw std::runtime_error("Parse<int> error");
		}
		return value;
	};
};

template <>
class Parser<Type::String> {
public:
	Parser() {};
	~Parser() {};

	Type::String parse(std::istream &stream) const {
		Type::String value;
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


#include "Parser.tpp"