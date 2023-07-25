#pragma once

#include "Types.hpp"

#include "Server.hpp"
#include <string>
#include <map>
#include <sys/socket.h>
#include <exception>
#include <netinet/in.h> // htons
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <vector>
#include "ResponseMessage.hpp"

class SocketManager {
private:
protected:
    SocketDetails clientSockets;

    SocketManager() {};

public:
    virtual ~SocketManager() {};
    virtual SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen) = 0;
    virtual void sendResponseMessage(int connectSd, ResponseMessage &responseMessage) const = 0;
};


class SelectSocketManager: public virtual SocketManager {
private:
public:
    fd_set readFds;
    SelectSocketManager() {};
    ~SelectSocketManager() {};
    SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen);
    virtual void sendResponseMessage(int connectSd, ResponseMessage &responseMessage) const;
};
