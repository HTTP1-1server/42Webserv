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
        const char *message = "\
HTTP/1.1 200 OK\r\n\
Server: Apache/2.2.14 (Win32)\r\n\
Content-Length: 53\r\n\
Content-Type: text/html\r\n\
Connection: close\r\n\
\n\
<html>\n\
<body>\n\
<h1>Hello, World!</h1>\n\
</body>\n\
</html>\n";

    while (1) {
        for (std::vector<Server>::iterator server = servers.begin(); server != servers.end(); ++server) {
            ConnectSd connectSd = server->acceptSdBy<Select>();
            if (connectSd < 0) {
                if(send(connectSd, message, strlen(message), 0) != strlen(message)) {  
                    perror("send");  
                }
                close(connectSd);
            }
        }
        
    }
    
}
