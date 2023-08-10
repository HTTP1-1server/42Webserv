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
    ServletApplication(const std::vector<ServerConfig> &configs);
    ~ServletApplication();

    void run();
};

ServletApplication::ServletApplication(const std::vector<ServerConfig> &configs): socketManager(new SelectSocketManager()), frontControllerServlet(configs) {
    for (std::vector<ServerConfig>::const_iterator conf = configs.begin(); conf != configs.end(); ++conf) {
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
                std::cout << "REQ URL: " << request.url << std::endl;
                frontControllerServlet.service(request, response);
                socketManager->sendResponseMessage(connectSd, response.getFullMessage());
            } else {
                if (connectSd != -1) {
                    std::cout << "Messages doesn't end. Need to receive more message " << connectSd << " ++||" << requestMessage << "++||"<< std::endl;
                    socketManager->clientSockets.insert(std::make_pair(connectSd, requestMessage));
                }
            }
        }
    }
}