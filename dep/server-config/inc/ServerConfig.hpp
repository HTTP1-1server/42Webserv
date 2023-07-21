#pragma once

#include "Parser.hpp"
#include <string>

class ServerConfig: public Tags {
public:
    ServerConfig(const Tags &other): Tags(other) {};

	static void parseListenTag(std::istream &stream, const std::string *key, Tags &tags) {
		int port;
        stream >> port;
        if (stream.fail()) {
            stream.clear();
            throw std::runtime_error("parse listen tag error");
        }
        Parser::skipLineEnd(stream);

        tags[*key] = new AnyType<int>(port);
	}

	static void parseHostTag(std::istream &stream, const std::string *key, Tags &tags) {
        std::string host;
        stream >> host;
        if (stream.fail() || host.empty()) {
            stream.clear();
            throw std::runtime_error("parse host tag error");
        }
        Parser::skipLineEnd(stream);
        if (*host.rbegin() == ';')
            host.resize(host.length() - 1);

        tags[*key] = new AnyType<std::string>(host);
    }

    static void parseLocationTag(std::istream &stream, const std::string *key, Tags &tags) {
        std::string path;
        stream >> path;
        if (stream.fail() || path.empty()) {
            stream.clear();
            throw std::runtime_error("parse location tag error");
        }
        Parser::skipLineEnd(stream);

        Any *locationGroup = new AnyType<Tags>(Tags());
        std::pair<Tags::iterator, bool> insertion = tags.insert(std::make_pair(*key, locationGroup));
        bool isDuplicated = !insertion.second;
		if (isDuplicated) {
            delete locationGroup;
		}

        Tags &locations = (Tags &)*insertion.first->second;
        
        Parser locationParser;
        locationParser.setFormat("allowed_methods", &ServerConfig::parseAllowedMethodsTag);

        locations[path] = new AnyType<Tags>(locationParser.parse(stream));
    }

	static void parseAllowedMethodsTag(std::istream &stream, const std::string *key, Tags &tags) {
        std::vector<std::string> methods;
        std::string line;
        std::getline(stream, line);
        if (stream.fail()) {
            stream.clear();
            throw std::runtime_error("parse allowed_methds tag error");
        }
        std::istringstream iss(line);
        while (iss.good()) {
            std::string method;
            iss >> method;
            if (iss.fail() || method.empty()) {
                throw std::runtime_error("parse allowed_methds tag error");
            }
            if (*method.rbegin() == ';')
                method.resize(method.length() - 1);
            methods.push_back(method);
        }
        if (methods.empty())
            throw std::runtime_error("parse allowed_methds tag error");
        tags[*key] = new AnyType<std::vector<std::string> >(methods);
    }
};
