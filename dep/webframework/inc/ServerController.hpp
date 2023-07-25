#pragma once

#include <vector>
#include <stdio.h>
#include <cstring>
#include <string>
#include "Server.hpp"
#include "Multiplexer.hpp"
#include "InputView.hpp"
#include "ResponseMessage.hpp"
#include "utils/ResponseHandler.hpp"
#include "RequestInfo.hpp"
#include "ResponseMessage.hpp"
#include "ServerConfig.hpp"

typedef int ConnectSd;

class ServerController {
private:
    std::vector<Server> servers;
    Multiplexer *multiplexer;
    const InputView *inputView;

    
public:
    ServerController(const std::vector<ServerConfig> &configs);
    ~ServerController();

    void run();
};

ServerController::ServerController(const std::vector<ServerConfig> &configs): multiplexer(new SelectMultiplexer()), inputView(new ConsoleInputView()) {
    for (std::vector<ServerConfig>::const_iterator conf = configs.begin(); conf != configs.end(); ++conf) {
        servers.push_back(Server(*conf));
    }
}

ServerController::~ServerController() {
    if (multiplexer)
        delete multiplexer;
}

typedef std::string Event;

bool isClientText(RequestMessage requestMessage) {
    return requestMessage.size() != 0;
}

void ServerController::run() {
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, RequestMessage> eventPair = multiplexer->detectEvent(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
            if (isClientText(eventPair.second)) {
                RequestInfo requestInfo(eventPair.second);
                ResponseHandler *responseHandler = generate("HTTP", server->config);

                std::pair<Code, Body> yongmin = responseHandler->processResponse(requestInfo);
                ResponseMessage responseMessage(yongmin);
                inputView->sendResponseMessage(eventPair.first, responseMessage);
                if (responseHandler)
                    delete responseHandler;
            }
        }
    }
}