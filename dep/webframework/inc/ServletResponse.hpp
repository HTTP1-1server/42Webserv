#pragma once

#include "Types.hpp"

#include <string>
#include <map>
#include "ServletRequest.hpp"


class ServletResponse: public std::string {
private:

public:
	int statusCode;
	std::map<std::string, std::string> header;
	std::string body;
	static const int OK = 200;
	static const int CREATED = 201;
	static const int NO_CONTENT = 204;
	static const int BAD_REQUEST = 400;
	static const int UNAUTHORIZED = 401;
	static const int PAYMENT_REQUIRED = 402;
	static const int FORBIDDEN = 403;
	static const int NOT_FOUND = 404;
	static const int SC_NOT_FOUND = 404;
	static const int METHOD_NOT_ALLOWED = 405;
	static const int TOO_LARGE = 413;
	static const int INTERNAL_SERVER_ERROR = 500;
	static const int NOT_IMPLEMENTED = 501;
	static const int BAD_GATEWAY = 502;
	static const int SERVICE_UNAVAILABLE = 503;

	ServletResponse(): statusCode(OK) {};

	void setStatus(int statusCode) {
		this->statusCode = statusCode;
	};
	
	void setBody(const std::string &content) {
		this->body = content;
	}

	std::string getProtocol() const {
		std::string status;
		if (statusCode == 200) {
			status = std::string("200 OK\r\n");
		} else if (statusCode == 400) {
			status = std::string("400 Bad Request\r\n");
		} else if (statusCode == 401) {
			status = std::string("401 Unauthorized\r\n");
		} else if (statusCode == 402) {
			status = std::string("402 Payment Required\r\n");
		} else if (statusCode == 403) {
			status = std::string("403 Forbidden\r\n");
		} else if (statusCode == 404) {
			status = std::string("404 Not Found\r\n");
		} else if (statusCode == 405) {
			status = std::string("405 Not Allowed Method\r\n");
		} else if (statusCode == 413) {
			status = std::string("413 Request Entity Too Large\r\n");
		}
		return "HTTP/1.1 " + status;
	}

	void addHeader(std::string key, std::string value) {
		this->header[key] = value;
	}

	std::string getFullMessage() const {
		std::string headers;
		if (this->header.find("Content-Type") == this->header.end()) {
			headers.append("Content-Type: text/html\r\n");
		}
		if (this->header.find("Content-Length") == this->header.end()) {
			std::string length;
			std::stringstream ss;
			ss << this->body.length();
			ss >> length;
			headers.append("Content-Length: " + length + "\r\n");
		}
		for (std::map<std::string, std::string>::const_iterator iter = this->header.begin();
			iter != this->header.end();
			++iter
		) {
			headers.append(iter->first + ": " + iter->second + "\r\n");
		}
		std::string message = this->getProtocol();
		message.append(headers + "\r\n" + this->body + "\r\n\r\n");
		return message;
	};
	
	~ServletResponse() {};
};

// TODO: status codes
// - 200
// - 405
//