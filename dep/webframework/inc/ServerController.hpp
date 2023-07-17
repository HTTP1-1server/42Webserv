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

typedef int ConnectSd;

class ServerController {
private:
    std::vector<Server> servers;
    Multiplexer *multiplexer;
    const InputView *inputView;

    
public:
    ServerController(const std::vector<Port> &ports);
    ~ServerController();

    void run();
};

ServerController::ServerController(const std::vector<Port> &ports): multiplexer(new SelectMultiplexer()), inputView(new ConsoleInputView()) {
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

    // View consoleInputView();

    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            std::pair<ConnectSd, Event> eventPair = multiplexer->detectEvent(server->listenSd, (struct sockaddr *)&server->sockAddr, (socklen_t *)&server->sockAddrLen);
            
            // 응답을 받았다면
            if (isClientText(eventPair.second)) {
                RequestInfo requestInfo(eventPair.second);
                // 응답을 처리한다
                // ResponseHandler *responseHandler = new HttpResponseHandler();
                // ResponseHandler *responseHandler = ResponseHandler::generate("HTTP");
                ResponseHandler *responseHandler = generate("HTTP");
                // ResponseMessage responseMessage = responseHandler->processResponse(requestInfo);

                std::pair<Code, Body> yongmin = responseHandler->processResponse(requestInfo);
                std::cout << "before response BODY: " << yongmin.second << std::endl;
                ResponseMessage responseMessage(yongmin);
                inputView->sendResponseMessage(eventPair.first, responseMessage);
            //     consoleInputView.sendResponse(responseMessage);
                if (responseHandler)
                    delete responseHandler;
            }
        }
    }
}