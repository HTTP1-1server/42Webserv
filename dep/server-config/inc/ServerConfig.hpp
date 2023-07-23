#pragma once

#include "Parser.hpp"
#include <string>

class ServerConfig: public Tags {
private:
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

public:
    ServerConfig(const Tags &other): Tags(other) {};

    static void parseServerBlock(std::istream &stream, const std::string *key, Tags &tags) {
        std::string server;
        stream >> server;
        if (stream.fail() || server.empty()) {
            stream.clear();
            throw std::runtime_error("parse error: " + *key);
        }
        Parser::skipLineEnd(stream);

        UniquePtr<Any> serverGroup = UniquePtr<Any>(AnyType<std::vector<ServerConfig> >(std::vector<ServerConfig>()));
        std::pair<Tags::iterator, bool> insertion = tags.insert(std::make_pair(*key, serverGroup));

        std::vector<ServerConfig> &servers = (std::vector<ServerConfig> &)*insertion.first->second.data;

        Parser serverParser;
        serverParser.setFormat("listen", &Parser::parseIntTag);
	    serverParser.setFormat("host", &Parser::parseStringTag);
	    serverParser.setFormat("location", &ServerConfig::parseLocationTag);

        servers.push_back(serverParser.parse(stream));
    }
};