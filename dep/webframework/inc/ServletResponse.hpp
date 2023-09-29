#pragma once

#include <string>
#include <map>

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

	ServletResponse();
	~ServletResponse();

	void setStatus(int statusCode);
	void setBody(const std::string &content);
	std::string getProtocol() const;
	void addHeader(std::string key, std::string value);
	std::string getFullMessage() const;
};