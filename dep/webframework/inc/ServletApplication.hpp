#pragma once

#include "Types.hpp"

#include <vector>
#include <stdio.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include "Server.hpp"
#include "SocketManager.hpp"
#include "utils/Handler.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "ServerConfig.hpp"
#include "FrontControllerServlet.hpp"

class ServletApplication {
private:
    std::vector<Server> servers;
    FrontControllerServlet frontControllerServlet;
    SocketManager *socketManager;
    
public:
    ServletApplication(std::vector<ServerConfig> &configs);
    ~ServletApplication();

    void run();
};

ServletApplication::ServletApplication(std::vector<ServerConfig> &configs): socketManager(new SelectSocketManager()), frontControllerServlet(configs) {
    for (std::vector<ServerConfig>::iterator conf = configs.begin(); conf != configs.end(); ++conf) {
        std::map<int, std::string> errorMap;
        errorMap.insert(std::make_pair(400, "./public/errors/400.html"));
        errorMap.insert(std::make_pair(401, "./public/errors/401.html"));
        errorMap.insert(std::make_pair(403, "./public/errors/403.html"));
        errorMap.insert(std::make_pair(404, "./public/errors/404.html"));
        errorMap.insert(std::make_pair(405, "./public/errors/405.html"));
        errorMap.insert(std::make_pair(413, "./public/errors/413.html"));
        errorMap.insert(std::make_pair(501, "./public/errors/501.html"));
        errorMap.insert(std::make_pair(502, "./public/errors/502.html"));
        std::map<int, std::string> &errors = *conf->at("error_page").data;
        for (std::map<int, std::string>::iterator iter = errors.begin(); iter != errors.end(); ++iter) {
            errorMap[iter->first] = iter->second;
        }
        errors = errorMap;
        servers.push_back(Server(*conf));
    }
}

ServletApplication::~ServletApplication() {
    if (socketManager)
        delete socketManager;
}

bool isSocketOk(const int &connectSd, const std::string &message) {
    return connectSd > 2 && !message.empty();
}

bool isMessageOk(const std::string &requestMessage) {
    size_t headerEnd = requestMessage.find("\r\n\r\n");
    if (headerEnd == std::string::npos) // need to receive more chunked or not chunked
        return false;
    // header exists
    std::stringstream headerStream(requestMessage.substr(0, headerEnd));
    std::string token;
    std::getline(headerStream, token); // remove "GET /url HTTP1.1\r\n"

    Parser httpHeaderParser;
    std::map<std::string, std::string> header = httpHeaderParser.parseHttpHeader(headerStream);
    if (!header["Transfer-Encoding"].empty() && header["Transfer-Encoding"].find("chunked") != std::string::npos) {
        std::string chunkedBody = requestMessage.substr(headerEnd, requestMessage.length() - headerEnd);
        if (chunkedBody.find("0\r\n\r\n") == std::string::npos)
            return false;
    }
    else if (!header["Content-Length"].empty()) {
        int bodyLength = std::atoi(header["Content-Length"].c_str());
        if (bodyLength != requestMessage.length() - headerEnd - 4)
            return false;
    }
    return true;
}

void ServletApplication::run() {
	int flag = 0;
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, RequestMessage> socketDetail = socketManager->getSocketDetail(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
            int &connectSd = socketDetail.first;
            std::string &requestMessage = socketDetail.second;

            if (isSocketOk(connectSd, requestMessage) && isMessageOk(requestMessage)) {
                ServletRequest request(requestMessage);
                ServletResponse response;
                std::map<int, std::string> &errors = *server->config.at("error_page").data;
                frontControllerServlet.service(server->config, request, response);
                socketManager->sendResponseMessage(connectSd, response.getFullMessage());
            } else {
                if (connectSd != -1) {
                    socketManager->clientSockets.insert(std::make_pair(connectSd, requestMessage));
                }
            }
        }
    }
}