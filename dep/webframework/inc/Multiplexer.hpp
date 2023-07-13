#pragma once

#include "Server.hpp"
#include <string>
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

#define MAX_CLIENTS (30)

typedef std::string Event;

class Multiplexer {
private:
protected:
    int numClients;
    int clientSockets[MAX_CLIENTS];
    fd_set readFds;

    Multiplexer() {};

public:
    virtual ~Multiplexer() {};
    virtual Event detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen) = 0;
};


class SelectMultiplexer: public virtual Multiplexer {
private:
public:
    SelectMultiplexer() {};
    ~SelectMultiplexer() {};
    Event detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen);
};
