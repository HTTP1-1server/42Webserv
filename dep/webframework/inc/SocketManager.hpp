#pragma once

#include <string>
#include "Types.hpp"


class SocketManager {
private:
protected:

    SocketManager() {};

public:
    SocketDetails clientSockets;

    virtual ~SocketManager() {};
    virtual SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen) = 0;
	virtual void sendResponseMessage(int connectSd, const std::string &responseMessage) = 0;
};


class SelectSocketManager: public virtual SocketManager {
private:
public:
    fd_set readFds;
    SelectSocketManager() {};
    ~SelectSocketManager() {};
    SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen);
	virtual void sendResponseMessage(int connectSd, const std::string &responseMessage);
};
