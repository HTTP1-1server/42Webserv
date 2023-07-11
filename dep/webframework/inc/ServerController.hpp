#pragma once

#include <vector>
#include <stdio.h>
#include <cstring>
#include "Server.hpp"

typedef int ConnectSd;

class ServerController {
private:
    std::vector<Server> servers;
public:
    ServerController(const std::vector<Port> &ports);
    ~ServerController();

    void run();
};

ServerController::ServerController(const std::vector<Port> &ports) {
    servers.clear();
    servers.reserve(ports.size());
    for (std::vector<Port>::const_iterator port = ports.begin(); port != ports.end(); ++port) {
        servers.push_back(Server(*port));
    }
}

ServerController::~ServerController() {
}

void ServerController::run() {

    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            ConnectSd connectSd = server->acceptSdBy<Select>();
        }
        
    }
    
}
