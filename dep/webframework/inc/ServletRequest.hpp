#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "SocketManager.hpp"
#include "Parser.hpp"

class ServletRequest {
private:
    // ServletRequest();

public:
    Method method;
    Body body;
    Url url;
    std::map<std::string, std::string> headers;
    // std::string root;

    std::string getRequestURI() const {
        return this->headers.at("Host") + this->url;
    }

    std::map<std::string, std::string> createParamMap() const {
        std::map<std::string, std::string> paramMap;
        paramMap.insert(std::make_pair("requestURL", this->url));
        paramMap.insert(std::make_pair("fullURL", this->getRequestURI()));
        if (this->headers.find("Content-Length") != this->headers.end()) {
            paramMap.insert(std::make_pair("Content-Length", this->headers.at("Content-Length")));
        }
        std::stringstream ss;
        ss << this->body.length();
        std::string bodyLengh;
        ss >> bodyLengh;
        paramMap.insert(std::make_pair("bodyLength", bodyLengh));

        if (this->body == "\r\n\r\n0\r\n\r\n") {
            paramMap.insert(std::make_pair("body", std::string()));
        } else {
            paramMap.insert(std::make_pair("body", this->body));
        }

        // TODO: body json to paramMap
        return paramMap;
    }
    
    ServletRequest(std::string requestMessage) {
		size_t headerEnd = requestMessage.find("\r\n\r\n");
		std::stringstream headerStream(requestMessage.substr(0, headerEnd));
        std::string token;

        headerStream >> token; // GET
        this->method = token;

        headerStream >> token; // url
        this->url = token;

		std::getline(headerStream, token);
		Parser httpHeaderParser;
		this->headers = httpHeaderParser.parseHttpHeader(headerStream);
		// for (std::map<std::string, std::string>::const_iterator iter = this->headers.begin(); iter != this->headers.end(); ++iter) {
        //     const std::string &headerKey = iter->first;
		// 	const std::string &headerValue = iter->second;
		// }
        this->body = requestMessage.substr(headerEnd, requestMessage.length() - headerEnd);
        // TODO: check body length where body.len() is equal to Content-Length of header 
        //      with the error, server should send and error page to the client
        
        // this->root = this->findRoot(this->url, config);
        
    };
    ~ServletRequest() {};

    std::string findRoot(const std::string &url, const ServerConfig &config) {
        std::string location;
        std::string root;

        if (config.find("location") == config.end()) {
            return *config.at("root").data;
        }
        const HashMap &locations = *config.at("location").data;

        for (HashMap::const_iterator locationIter = locations.begin(); locationIter != locations.end(); ++locationIter) {
            if (url.find(locationIter->first) != std::string::npos) {
                if (locationIter->first.length() >= location.length()) {
                    const HashMap &locationConfig = *locationIter->second.data;
                    if (locationConfig.find("root") != locationConfig.end()) {
                        std::string locationRoot = *locationConfig.at("root").data;
                        root = locationRoot;
                    } else {
                        std::string serverRoot = *config.at("root").data;
                        root = serverRoot;
                    }
                }
            }
        }
        if (root.empty())
            return *config.at("root").data;
        return root;
    }
};