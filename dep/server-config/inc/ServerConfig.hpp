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

        UniquePtr<Any> locationGroup = UniquePtr<Any>(AnyType<Tags>(Tags()));
        std::pair<Tags::iterator, bool> insertion = tags.insert(std::make_pair(*key, locationGroup));

        Tags &locations = (Tags &)*insertion.first->second.data;
        
        Parser locationParser;
        locationParser.setFormat("allowed_methods", &Parser::parseVecOfStringTag);

        locations.insert(std::make_pair(path, UniquePtr<Any>(AnyType<Tags>(locationParser.parse(stream)))));
    }
};
