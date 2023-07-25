#pragma once 

#include <string>
#include <map>

typedef std::string Method;
typedef std::string Url;
typedef std::string Body;
typedef int         Code;

typedef std::pair<Code, Body> Response;

typedef int							ConnectSd;
typedef std::string RequestMessage;
typedef std::pair<ConnectSd, RequestMessage> SocketDetail;
typedef std::map<ConnectSd, RequestMessage>	SocketDetails;

