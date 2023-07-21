#pragma once

#include "Parser.hpp"
#include <string>

class ServerConfig: public Tags {
public:
    ServerConfig(const Tags &other): Tags(other) {};

    static void parseLocationTag(std::istream &stream, const std::string *key, Tags &tags) {
        std::string path;
        stream >> path;
        if (stream.fail() || path.empty()) {
            stream.clear();
            throw std::runtime_error("parse error: " + *key);
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
        locationParser.setFormat("allowed_methods", &Parser::parseVecOfStringTag);

        locations[path] = new AnyType<Tags>(locationParser.parse(stream));
    }
};
