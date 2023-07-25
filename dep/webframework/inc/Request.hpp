#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "SocketManager.hpp"

class Request
{
private:
    Request();

public:
    Method method;
    Body body;
    Url url;

    Request(RequestMessage requestMessage) {
		
		std::stringstream sstream(requestMessage);
        std::string token;

        sstream >> token; // GET
        this->method = token;

        sstream >> token; // url
        this->url = token;
    };
    ~Request() {};
};