#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "SocketManager.hpp"
#include "Parser.hpp"

class Request
{
private:
    Request();

public:
    Method method;
    Body body;
    Url url;

    Request(std::string requestMessage) {
		size_t headerEnd = requestMessage.find("\r\n\r\n");
		std::stringstream headerStream(requestMessage.substr(0, headerEnd));
        std::string token;

        headerStream >> token; // GET
        this->method = token;

        headerStream >> token; // url
        this->url = token;

		std::getline(headerStream, token);
		Parser httpHeaderParser;
		std::map<std::string, std::string> header = httpHeaderParser.parseHttpHeader(headerStream);
		for (std::map<std::string, std::string>::const_iterator iter = header.begin(); iter != header.end(); ++iter) {
			const std::string &value = iter->second;
			std::cout << "K::" << iter->first << " V::" << value << std::endl;
		}
        this->body = requestMessage.substr(headerEnd, requestMessage.length() - headerEnd);
    };
    ~Request() {};
};