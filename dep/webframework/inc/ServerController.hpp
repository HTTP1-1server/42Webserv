#pragma once

#include <vector>
#include <stdio.h>
#include <cstring>
#include <string>
#include "Server.hpp"
#include "Multiplexer.hpp"

typedef int ConnectSd;

class ServerController {
private:
    std::vector<Server> servers;
    Multiplexer *multiplexer;
public:
    ServerController(const std::vector<Port> &ports);
    ~ServerController();

    void run();
};

ServerController::ServerController(const std::vector<Port> &ports): multiplexer(new SelectMultiplexer()) {
    servers.clear();
    servers.reserve(ports.size());
    for (std::vector<Port>::const_iterator port = ports.begin(); port != ports.end(); ++port) {
        servers.push_back(Server(*port));
    }
}

ServerController::~ServerController() {
    if (multiplexer)
        delete multiplexer;
}

// ServerController constructor (conf) -> setting
typedef std::string Event;

bool isClientText(Event &event) {
    return event.length() != 0;
}

void ServerController::run() {
    
    // 1. ServerMaker (socket -> bind -> listen)
    //while (1)
    // 2. ServerEventDetector (select -> accept -> FD_ISSET) -> string msg;
    
    // 3-1. static utils/HTTPFunctionHandler -> HTTP 1.1 (FunctionHandler)
    // 3-2. ServerResponseMessage -> ResponseMessage



    // 4 View(ResponseMessage) // ConsoleInputView (InputView)

    // 생성자에서 주입
    // SelectMultiplexer selectMultiplexer; // selectMultiplexer
    // ResponseHandler responseHandler("HTTP");
    // View consoleInputView();
    
    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            Event event = multiplexer->detectEvent(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
            // //응답을 받았다면
            // if (isClientText(event)) {
            //     RequestInfo requestInfo(event);
            //     //응답을 처리한다
            //     ResponseMessage responseMessage = responseHandler.processResponse(requestInfo);
                
            //     consoleInputView.sendResponse(responseMessage);
            // }
        }
    }
}