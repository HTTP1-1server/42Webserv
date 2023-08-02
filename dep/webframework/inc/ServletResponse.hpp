#pragma once

#include "Types.hpp"

#include <string>
#include "ServletRequest.hpp"


class ServletResponse: public std::string {
private:
	int statusCode;
public:
	static const int OK = 200;
	static const int CREATED = 201;
	static const int NO_CONTENT = 204;
	static const int BAD_REQUEST = 400;
	static const int UNAUTHORIZED = 401;
	static const int FORBIDDEN = 403;
	static const int NOT_FOUND = 404;
	static const int SC_NOT_FOUND = 404;
	static const int METHOD_NOT_ALLOWED = 405;
	static const int INTERNAL_SERVER_ERROR = 500;
	static const int SERVICE_UNAVAILABLE = 503;

	ServletResponse(): statusCode(OK) {};

	void setStatus(int statusCode) {
		this->statusCode = statusCode;
	};
	// ServletResponse(Response response) {
	// 	const std::string PROTOCOL("HTTP/1.1 ");
	// 	std::string code;
	// 	std::string status;
	// 	if (response.first == 200) {
	// 		code = std::string("200 ");
	// 		status = std::string("OK\r\n");
	// 	} else if (response.first == 405) {
	// 		code = std::string("405 ");
	// 		status = std::string("Not Allowed Method\r\n");
	// 	} else if (response.first == 401) {
	// 		code = std::string("405 ");
	// 		status = std::string("Not Allowed Method\r\n");
	// 	}
	// 	const std::string CONTENT_TYPE("Content-Type: text/html\r\n");

	// 	std::string responseHeader = PROTOCOL + code + status;
	// 	std::string responseBody = response.second;

	// 	this->append(responseHeader + std::string("\r\n"));// + responseBody);
	// };
	~ServletResponse() {};
};

// TODO: status codes
// - 200
// - 405
//