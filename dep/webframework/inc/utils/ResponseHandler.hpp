#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include "Multiplexer.hpp"
#include "RequestInfo.hpp"

typedef std::string Event;
typedef std::string Body;

class HttpResponseHandler;

class ResponseHandler {
protected:
    ResponseHandler() {};
public:
    virtual ~ResponseHandler() {};
    virtual std::pair<Code, Body> processResponse(RequestInfo eventMessage) = 0;
};

std::pair<Code, Body> staticWebpage(std::string filename) {
    std::string filename2 = filename;
    if (filename.size() >= 1 && filename[0] == '/')
        filename2 = filename.substr(1, filename.size() - 1);
    std::fstream fs(filename2.c_str());
    std::stringstream buffer;
    buffer << fs.rdbuf();
    // if (처리실패)
    //     return std::make_pair(400, ERROR_MESSAGE); 
    return std::make_pair(200, buffer.str());
};

class HttpResponseHandler: public virtual ResponseHandler {
private:
public:
    HttpResponseHandler() {};
    ~HttpResponseHandler() {};
    std::pair<Code, Body> processResponse(RequestInfo eventMessage) {
        // 0. parse message -> memberValue

        // std::map<"key", &Fn> fnTable;
        // fnTable[("GET", "/")] = &staticWebpate;

        // 1. method check 
        //  GET(static) -> 
        //  GET(image)
        //  GET(CGI)
        //  POST(image)
        //  POST(CGI)
        //  DELETE(image)

        // 2. method exec
        std::cout << "URL: " << eventMessage.url << std::endl;
        return staticWebpage(eventMessage.url);
    };
};

ResponseHandler *generate(std::string protol) {
    if (protol == "HTTP")
        return new HttpResponseHandler();
    return 0;
    // if (protol == "HTTPS")
    //     return new HttpsResponseHandler();
}