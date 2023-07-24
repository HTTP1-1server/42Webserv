#pragma once

#include "Parser.hpp"
#include <string>

class ServerConfig: public Tags {
private:
    static void parseLocationTag(std::istream &stream, const std::string *key, Tags &tags);
    static void parseErrorTag(std::istream &stream, const std::string *key, Tags &tags);

public:
    ServerConfig(const Tags &other);

    static void parseServerBlock(std::istream &stream, const std::string *key, Tags &tags);
};