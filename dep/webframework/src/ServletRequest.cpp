#include "ServletRequest.hpp"

std::string ServletRequest::getRequestURI() const {
    return this->headers.at("Host") + this->url;
}

std::map<std::string, std::string> ServletRequest::createParamMap(const std::string &requestRoot) const {
    std::map<std::string, std::string> paramMap;
    paramMap.insert(std::make_pair("requestURL", this->url));
    std::string requestPath = requestRoot;
    const size_t extensionPos = requestPath.find_last_of("*.");
    size_t colonPos = requestPath.find_last_of(":");
    if (extensionPos != std::string::npos && colonPos != std::string::npos && colonPos < extensionPos) {
        requestPath = requestPath.substr(0, extensionPos);
    }
    paramMap.insert(std::make_pair("requestRoot", requestPath));
    int pos = this->getRequestURI().find(requestPath);

    std::string restOfRequest = this->getRequestURI().substr(pos + requestPath.length(), this->getRequestURI().length() - pos - requestPath.length());
    paramMap.insert(std::make_pair("restOfRequest", restOfRequest));
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

    paramMap.insert(std::make_pair("method", this->method));

    if (headers.find("X-Secret-Header-For-Test") != headers.end()) {
        paramMap.insert(std::make_pair("secretHeader", headers.at("X-Secret-Header-For-Test")));
    }
    return paramMap;
}

ServletRequest::ServletRequest(std::string requestMessage) {
    size_t headerEnd = requestMessage.find("\r\n\r\n");
    std::stringstream headerStream(requestMessage.substr(0, headerEnd));
    std::string token;

    headerStream >> token; // GET
    this->method = token;

    headerStream >> token; // url
    this->url = token;

    std::getline(headerStream, token);
    Parser httpParser;
    this->headers = httpParser.parseHttpHeader(headerStream);
    
    this->body = requestMessage.substr(headerEnd, requestMessage.length() - headerEnd);
    if (this->headers.find("Transfer-Encoding") != this->headers.end() && this->headers["Transfer-Encoding"] == "chunked") {
        this->body = httpParser.parseChunkedBody(this->body);
    }
};
ServletRequest::~ServletRequest() {};

std::string ServletRequest::findRoot(const std::string &url, const ServerConfig &config) {
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