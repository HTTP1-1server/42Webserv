#pragma once

#include <string>
#include "ServerConfig.hpp"
#include "Types.hpp"

class ServletRequest {
private:
public:
    Method method;
    Body body;
    Url url;
    std::map<std::string, std::string> headers;

    ServletRequest(std::string requestMessage);
    ~ServletRequest();

    std::string getRequestURI() const;
    std::map<std::string, std::string> createParamMap(const std::string &requestRoot) const;
    std::string findRoot(const std::string &url, const ServerConfig &config);
};