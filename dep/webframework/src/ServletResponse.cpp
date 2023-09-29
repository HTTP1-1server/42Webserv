#include "ServletResponse.hpp"
#include <sstream>

void ServletResponse::setStatus(int statusCode) {
	this->statusCode = statusCode;
};

void ServletResponse::setBody(const std::string &content) {
	this->body = content;
}

std::string ServletResponse::getProtocol() const {
	std::string status;
	if (statusCode == 200) {
		status = std::string("200 OK\r\n");
	} else if (statusCode == 303) {
		status = std::string("303 Found\r\n");
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
	} else if (statusCode == 501) {
		status = std::string("501 S2501 \r\n");
	}
	return "HTTP/1.1 " + status;
}

void ServletResponse::addHeader(std::string key, std::string value) {
	this->header[key] = value;
}

std::string ServletResponse::getFullMessage() const {
	std::string headers;
	if (this->header.find("Content-Type") == this->header.end()) {
		headers.append("Content-Type: text/html\r\n");
	}
	if (this->header.find("Connection") == this->header.end()) {
		headers.append("Connection: close\r\n");
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
	message.append(headers + "\r\n" + this->body);
	return message;
};


ServletResponse::ServletResponse(): statusCode(OK) {};
ServletResponse::~ServletResponse() {};