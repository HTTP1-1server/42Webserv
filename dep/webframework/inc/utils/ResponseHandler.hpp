#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include "Multiplexer.hpp"
#include "RequestInfo.hpp"
#include <vector>
#include <algorithm>
#include <iostream>


typedef std::string Event;

class HttpResponseHandler;

class ResponseHandler {
protected:
    ResponseHandler() {};
public:
    virtual ~ResponseHandler() {};
    virtual std::pair<Code, Body> processResponse(RequestInfo requestMessage) = 0;
};

class HttpResponseHandler: public virtual ResponseHandler {
private:
public:
	int code;
	std::string fileName;
	const ServerConfig &config;
	
    HttpResponseHandler(const ServerConfig &config): config(config) {};
    ~HttpResponseHandler() {};
    std::pair<Code, Body> processResponse(RequestInfo requestMessage) {

		std::string requestURL = requestMessage.url;
		std::string requestMethod = requestMessage.method;


		if (requestMessage.url == "/") {
			std::string index = *config.at("index").data;
			fileName = "public/" + index;
			this->code = 200;
		} else {
			std::cout << "preparing~~~~~~~!!!!!" << std::endl;
		}
		
        return staticWebpage(fileName);
    };

	std::pair<Code, Body> staticWebpage(std::string filename) {
		std::string filename2 = filename;
		if (filename.size() >= 1 && filename[0] == '/')
			filename2 = filename.substr(1, filename.size() - 1);
		std::fstream fs(filename2.c_str());
		
		std::ostringstream buffer;
		buffer << fs.rdbuf();
		fs.close();
		return std::make_pair(this->code, buffer.str());
	};
};

ResponseHandler *generate(std::string protol, const ServerConfig &config) {
    if (protol == "HTTP")
        return new HttpResponseHandler(config);
    return 0;
    // if (protol == "HTTPS")
    //     return new HttpsResponseHandler();
}