#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include "Multiplexer.hpp"

typedef std::string Method;
typedef std::string Path;
typedef std::string Body;

class RequestInfo
{
private:
    Method method;
    Path location;
    Body body;
    RequestInfo();

public:
    Path url;

    RequestInfo(Event eventMessage) {
        std::stringstream sstream(eventMessage);
        std::string token;

        sstream >> token; // GET
        this->method = token;
        sstream >> token; // url
        this->url = token;
        // this->location = 
        std::getline(sstream, this->body, '\0');

        // event message example
        //
        //    GET / HTTP/1.1
        //    Host: localhost:8889
        //    User-Agent: curl/7.81.0
        //    Accept: */*
        //
        //    post / HTTP/1.1
        //    Host: localhost:8888
        //    User-Agent: curl/7.81.0
        //    Accept: */*
        //    Content-Length: 7
        //    Content-Type: application/x-www-form-urlencoded
        //    \n
        //    hithere
        //
        //    post /static/kk HTTP/1.1
        //    Host: localhost:8888
        //    User-Agent: curl/7.81.0
        //    Accept: */*
        //    Content-Length: 7
        //    Content-Type: application/x-www-form-urlencoded
        //
        //    hithere
        //
        // parse method (GET, POST, DELELTE)

        // parse location (<location>, ex) /static, /image, /cgi)
        // parse url (ex. /domain/static, /domain/image/hihi, /domain/cgi?what=value)
        // parse body
    };
    ~RequestInfo() {};
};
