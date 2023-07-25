#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "Multiplexer.hpp"

typedef std::string Method;
typedef std::string Path;
typedef std::string Body;

// int cook(std::istream &stream) {
//     while (!stream.fail()) {
//         std::string header;
//         std::getline(stream, header);
//         if (header.length() < 1) {
//             stream.clear();
//             return stream.tellg();
//         }
//         if (header[header.size() - 1] == '\r') {
//             continue;
//         } else {
//             stream.clear();
//             return stream.tellg();
//         }
//     }
//     return stream.tellg();
// };

class RequestInfo
{
private:
    RequestInfo();

public:
    Method method;
    Body requestBody;
    Path url;

    RequestInfo(RequestMessage requestMessage) {
		
		std::stringstream sstream(&requestMessage[0]);
        std::string token;

        sstream >> token; // GET
        this->method = token;

        sstream >> token; // url
        this->url = token;
    };
    ~RequestInfo() {};
};