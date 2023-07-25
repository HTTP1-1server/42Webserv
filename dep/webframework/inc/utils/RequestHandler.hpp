#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include "SocketManager.hpp"
#include "Request.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

class RequestHandler {
protected:
    RequestHandler() {};
public:
	static RequestHandler * generate(std::string protocol, const ServerConfig &config);

    virtual ~RequestHandler() {};
    virtual Response handle(Request request) = 0;
};

class HttpRequestHandler: public virtual RequestHandler {
private:
public:
	int code;
	std::string fileName;
	const ServerConfig &config;
	
    HttpRequestHandler(const ServerConfig &config): config(config) {};
    ~HttpRequestHandler() {};
    Response handle(Request request) {

		std::string requestURL = request.url;
		std::string requestMethod = request.method;


		if (request.url == "/") {
			std::string index = *config.at("index").data;
			fileName = "public/" + index;
			this->code = 200;
		} else {
			std::cout << "preparing~~~~~~~!!!!!" << std::endl;
		}
		
        return staticWebpage(fileName);
    };

	Response staticWebpage(std::string filename) {
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

RequestHandler * RequestHandler::generate(std::string protocol, const ServerConfig &config) {
	if (protocol == "HTTP")
		return new HttpRequestHandler(config);
	return 0;
}