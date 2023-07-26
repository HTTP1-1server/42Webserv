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

bool isSocketOk(const int &connectSd, const std::string &message) {
    return connectSd > 2 && !message.empty();
}

void ServerController::run() {
	int flag = 0;
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, RequestMessage> socketDetail = socketManager->getSocketDetail(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
			int &connectSd = socketDetail.first;
			std::string &requestMessage = socketDetail.second;

            if (isSocketOk(connectSd, requestMessage)) {
				try {
					std::cout << requestMessage << std::endl;
                	Request request(requestMessage);
					RequestHandler *requestHandler = RequestHandler::generate("HTTP", server->config);

					Response response = requestHandler->handle(request);
					ResponseMessage responseMessage(response);
					socketManager->sendResponseMessage(connectSd, responseMessage);
					if (requestHandler)
						delete requestHandler;
				}
				catch(const std::exception& e) {
					std::cerr << e.what() << '\n';
					socketManager->clientSockets.insert(std::make_pair(connectSd, RequestMessage()));
				}
            } else {
                if (connectSd != -1) {
                    std::cerr << "something wrong happen in socket" << '\n';
                    close(connectSd);
                }
            }
        }
    }
}