#pragma once

#include "ResponseMessage.hpp"

class ResponseHandler {
protected:
    ResponseHandler() {};
public:
    virtual ~ResponseHandler() {};
    virtual ResponseMessage processResponse(Event eventMessage) = 0;
};

class HttpResponseHandler: public virtual ResponseHandler {
private:
public:
    HttpResponseHandler();
    ~HttpResponseHandler();
    ResponseMessage processResponse(Event eventMessage) {
        // 0. parse message -> memberValue

        // 1. method check 
        //  GET(static)
        //  GET(image)
        //  GET(CGI)
        //  POST(image)
        //  POST(CGI)
        //  DELETE(image)

        // 2. method exec
    };
};

HttpResponseHandler::HttpResponseHandler() {
}

HttpResponseHandler::~HttpResponseHandler() {
}
