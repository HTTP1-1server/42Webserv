#pragma once

#include <vector>
#include "Server.hpp"
#include "SocketManager.hpp"
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
