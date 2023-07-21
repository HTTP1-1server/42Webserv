#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "Multiplexer.hpp"

typedef std::string Method;
typedef std::string Path;
typedef std::vector<char> Dody;

int cook(std::istream &stream) {
    while (!stream.fail()) {
        std::string header;
        std::getline(stream, header);
        if (header.length() < 1) {
            stream.clear();
            return stream.tellg();
        }
        if (header[header.size() - 1] == '\r') {
            continue;
        } else {
            stream.clear();
            return stream.tellg();
        }
    }
    return stream.tellg();
};

class RequestInfo
{
private:
    RequestInfo();

public:
    Method method;
    Path location;
	std::string boundary;
    Dody dody;
    Path url;

    RequestInfo(RequestMessage requestMessage) {
		requestMessage.push_back('\0');
		std::stringstream sstream(&requestMessage[0]);
        std::string token;

        sstream >> token; // GET
        this->method = token;

        sstream >> token; // url
        this->url = token;
		
		this->boundary = extractBoundary(requestMessage);

        int pos = cook(sstream);
        std::vector<char>::iterator start = requestMessage.begin() + pos;
        std::vector<char>::iterator end = requestMessage.end();
        
		std::vector<char> dody(start, end);
		this->dody = dody;

        // std::cout << "~~~~~~~~~~~~~~~~~~~~~~!!!!!!!!!!!!!!!!!!!\n";
        // for (; start != end; ++start) {
        //     std::cout << *start;
        // }
        // std::cout << "!!!!!!!!!!!!!!!!!!!~~~~~~~~~~~~~~~~~~~~~~\n" << std::endl;
        // std::cout << "~~~~~~~~~~~~~~~~~~~~~~VECTOR!!!!!!!!!!!!!!!!!!!\n";

        // for (std::vector<char>::const_iterator iter = dody.begin(); iter != dody.end(); ++iter) {
        //     std::cout << *iter;
        // }
        // std::cout << "!!!!!!!!!!!!!!!!!!!VECTOr~~~~~~~~~~~~~~~~~~~~~~" << dody.size() << std::endl;
		// dody.push_back('\0');  // '\0' 문자 추가
		// char* ch = &dody[0];
        // std::cout << ch << std::endl;
        // std::cout << "~~~~~~~~~~~~~~~~~~~~~~VECTOR3!!!!!!!!!!!!!!!!!!!\n";
        // for (int i = 0; i < dody.size(); ++i) {
        //     std::cout << dody.data()[i];
        // }
        // std::cout << "!!!!!!!!!!!!!!!!!!!VECTOr3~~~~~~~~~~~~~~~~~~~~~~" << dody.size() << std::endl;
        // char buffer[10000];
        // memcpy(&buffer, dody.data(), dody.size());
        // buffer[dody.size()] = '\0';
        // std::cout << "~~~~~~~~~~~~~~~~~~~~~~buffer!!!!!!!!!!!!!!!!!!!\n";
        // std::cout << buffer << std::endl;
        // // for (int i = 0; i < dody.size(); ++i) {
        // //     std::cout << buffer[i];
        // // }
        // std::cout << "!!!!!!!!!!!!!!!!!!!buffer~~~~~~~~~~~~~~~~~~~~~~" << dody.size() << std::endl;

        // std::time_t t = std::time(0);
        // std::stringstream ss;
        // ss << "public/ssp" << t << ".png";
        // // std::ofstream fs(ss.str().c_str(), std::ios::binary);
        // // fs.write(dody.data(), dody.size());
        // // fs.close();
        // int fd = open(ss.str().c_str(), O_CREAT | O_WRONLY);
        // write(fd, dody.data(), dody.size());
        // close(fd);

        // std::ofstream fs(ss.str().c_str(), std::ios::binary | std::ios::out);
        // fs.
		// // for (std::vector<char>::const_iterator iter = dody.begin(); iter != dody.end(); ++iter) {
        // //     fs.put(*iter);
        // //     //std::cout << "WTF: " << *iter << std::endl;
        // // }
        // fs.write(dody.data(), dody.size() * sizeof(wchar_t));
		// // fs.flush();
        // fs.close();
		// this->body = dody;

		// requestMessage.push_back('\0');
        // std::stringstream sstream(&requestMessage[0]);
        // std::string token;

        // sstream >> token; // GET
        // this->method = token;
        // sstream >> token; // url
        // this->url = token;
        // // this->location = 
        // std::getline(sstream, this->body, '\0');

        // parse location (<location>, ex) /static, /image, /cgi)
        // parse url (ex. /domain/static, /domain/image/hihi, /domain/cgi?what=value)
        // parse body
    };
    ~RequestInfo() {};

	// void toString() {
	// 	std::cout << "\n-----The whole request message-----\n";
	// 	std::cout << this->method << " " << this->url << this->body << std::endl;
	// };

	std::string extractBoundary(const std::vector<char>& requestData) {
		std::string boundaryStr;
		std::string contentTypeHeader = "Content-Type: multipart/form-data; boundary=";

		// Convert the request data vector to a string for easier processing
		std::string requestDataStr(requestData.begin(), requestData.end());

		// Find the position of the Content-Type header
		size_t contentTypePos = requestDataStr.find(contentTypeHeader);

		if (contentTypePos != std::string::npos) {
			// Extract the boundary value from the Content-Type header
			size_t boundaryStart = contentTypePos + contentTypeHeader.length();
			size_t boundaryEnd = requestDataStr.find("\r\n", boundaryStart);

			if (boundaryEnd != std::string::npos) {
				boundaryStr = requestDataStr.substr(boundaryStart, boundaryEnd - boundaryStart);
			}
		}

		return boundaryStr;
	}
};