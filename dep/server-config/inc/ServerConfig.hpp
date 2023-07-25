#pragma once

#include "Types.hpp"

#include "Parser.hpp"
#include <string>

class ServerConfig: public HashMap {
private:
    static void parseLocationTag(std::istream &stream, const std::string *key, HashMap &tags);
    static void parseErrorTag(std::istream &stream, const std::string *key, HashMap &tags);

public:
    ServerConfig(const HashMap &other);

    static void parseServerBlock(std::istream &stream, const std::string *key, HashMap &tags);
};