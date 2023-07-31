#pragma once

#include "Types.hpp"

#include <vector>
#include <stdio.h>
#include <cstring>
#include <string>
#include <cstdlib>
#include "Server.hpp"
#include "SocketManager.hpp"
#include "utils/RequestHandler.hpp"
#include "Request.hpp"
#include "ResponseMessage.hpp"
#include "ServerConfig.hpp"


class ServerController {
private:
    std::vector<Server> servers;
    SocketManager *socketManager;
    
public:
    ServerController(const std::vector<ServerConfig> &configs);
    ~ServerController();

    void run();
};

ServerController::ServerController(const std::vector<ServerConfig> &configs): socketManager(new SelectSocketManager()) {
    for (std::vector<ServerConfig>::const_iterator conf = configs.begin(); conf != configs.end(); ++conf) {
        servers.push_back(Server(*conf));
    }
}

ServerController::~ServerController() {
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

void ServerController::run() {
	int flag = 0;
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, RequestMessage> socketDetail = socketManager->getSocketDetail(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
			int &connectSd = socketDetail.first;
			std::string &requestMessage = socketDetail.second;

            if (isSocketOk(connectSd, requestMessage) && isMessageOk(requestMessage)) {
                Request request(requestMessage);
                RequestHandler *requestHandler = RequestHandler::generate("HTTP", server->config);

                Response response = requestHandler->handle(request);
                ResponseMessage responseMessage(response);
                socketManager->sendResponseMessage(connectSd, responseMessage);
                if (requestHandler)
                    delete requestHandler;
            } else {
                if (connectSd != -1) {
                    std::cout << "Messages doesn't end. Need to receive more message" << std::endl;
                    socketManager->clientSockets.insert(std::make_pair(connectSd, requestMessage));
                }
            }
        }
    }
}