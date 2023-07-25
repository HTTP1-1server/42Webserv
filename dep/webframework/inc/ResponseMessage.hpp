#pragma once

#include "Types.hpp"

#include <string>
#include "Request.hpp"

class ResponseMessage: public std::string {
public:
	ResponseMessage(Response response) {
		const std::string PROTOCOL("HTTP/1.1 ");
		const std::string CODE_OK("200 ");
		const std::string STATUS_OK("OK\r\n");
		const std::string CONTENT_TYPE("Content-Type: text/html\r\n");

		std::string responseHeader = PROTOCOL + CODE_OK + STATUS_OK + CONTENT_TYPE;
		std::string responseBody = response.second;

		this->append(responseHeader + std::string("\n") + responseBody);
	};
	~ResponseMessage() {};
};
