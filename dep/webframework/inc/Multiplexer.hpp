#pragma once

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

typedef int							ConnectSd;
typedef std::string					RequestMessage;
typedef std::map<ConnectSd, RequestMessage>	ClientSocketInfo;

class Multiplexer {
private:
protected:
    ClientSocketInfo clientSocketInfo;

    Multiplexer() {};

public:
    virtual ~Multiplexer() {};
    virtual std::pair<ConnectSd, RequestMessage> detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen) = 0;
};


class SelectMultiplexer: public virtual Multiplexer {
private:
public:
    fd_set readFds;
    SelectMultiplexer() {};
    ~SelectMultiplexer() {};
    std::pair<ConnectSd, RequestMessage> detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen);
	
};
