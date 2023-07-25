#pragma once

#include "Types.hpp"

#include <vector>
#include <stdio.h>
#include <cstring>
#include <string>
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

bool isClientText(RequestMessage requestMessage) {
    return requestMessage.size() != 0;
}

void ServerController::run() {
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, RequestMessage> socketDetail = socketManager->getSocketDetail(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
			int &connectSd = socketDetail.first;
			std::string &requestMessage = socketDetail.second;

            if (isClientText(requestMessage)) {
                Request request(requestMessage);
                RequestHandler *requestHandler = RequestHandler::generate("HTTP", server->config);

                Response response = requestHandler->handle(request);
                ResponseMessage responseMessage(response);
                socketManager->sendResponseMessage(connectSd, responseMessage);
                if (requestHandler)
                    delete requestHandler;
            }
        }
    }
}