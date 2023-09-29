#pragma once

#include <string>
#include "Types.hpp"


class SocketManager {
private:
protected:

    SocketManager() {};

public:
    SocketDetails recvSockets;
    SocketDetails sendSockets;

    virtual ~SocketManager() {};
    virtual SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen) = 0;
	virtual void addResponseMessage(int connectSd, const std::string &responseMessage) = 0;
    virtual void sendResponseMessage() = 0;
};


class SelectSocketManager: public virtual SocketManager {
private:
public:
    fd_set readFds;
    SelectSocketManager() {};
    ~SelectSocketManager() {};
    SocketDetail getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen);
	virtual void addResponseMessage(int connectSd, const std::string &responseMessage);
    virtual void sendResponseMessage();
};
