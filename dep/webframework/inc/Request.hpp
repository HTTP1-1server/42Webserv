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
	int flag;

    Request(std::string requestMessage) {
		this->flag = 0;
		size_t headerEnd = requestMessage.find("\r\n\r\n");
		if (headerEnd == std::string::npos || headerEnd == 0) 
			throw std::runtime_error("header error");
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

		if (isConnectionKeepAlive(requestMessage)) {
			this->flag = 1;
		}

		std::cout << "[" << requestMessage << "]" << std::endl;
    };
    ~Request() {};

	bool isConnectionKeepAlive(const std::string& requestHeaders) {
    // 요청 헤더를 개행문자(CRLF)로 분리
    size_t startPos = 0;
    size_t endPos = 0;
    std::string delimiter = "\r\n";
    while ((endPos = requestHeaders.find(delimiter, startPos)) != std::string::npos) {
        std::string line = requestHeaders.substr(startPos, endPos - startPos);
        startPos = endPos + delimiter.length();

        // Connection 헤더를 찾으면 연결 유지 여부를 확인
        if (line.find("Connection: ") == 0) {
            std::string value = line.substr(12); // "Connection: " 이후의 값을 가져옴
			std::cout << "Found CONNECTION " << value << std::endl;
            // return (value == "keep-alive");
            return (value == "close");
        }
    }

    return false; // Connection 헤더가 없으면 기본적으로 연결을 끊습니다.
}
};