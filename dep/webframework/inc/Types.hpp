#pragma once 

#include <string>
#include <map>
#include <netinet/in.h>

typedef int                 Port;
typedef int                 ListenSd;
typedef struct sockaddr_in6 SocketAddrV6;

typedef std::string Method;
typedef std::string Url;
typedef std::string Body;
typedef int Code;

typedef int	ConnectSd;
typedef std::string RequestMessage;
typedef std::pair<ConnectSd, RequestMessage> SocketDetail;
typedef std::map<ConnectSd, RequestMessage>	SocketDetails;

