#include "Multiplexer.hpp"

std::pair<ConnectSd, Event> SelectMultiplexer::detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
	int maxSd = listenSd;
	
	for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
		if (iter->first > maxSd)
			maxSd = iter->first;

	FD_ZERO(&readFds);  
	FD_SET(listenSd, &readFds);

	for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
		FD_SET(iter->first, &readFds);

	struct timeval tv = {0, 0};
	int activity = select(maxSd + 1 , &readFds , NULL , NULL , &tv);
	if (activity > 0) {
		if (FD_ISSET(listenSd, &readFds)) {
			int connectSd = accept(listenSd, sockAddr, sockAddrLen);
			if (connectSd >= 0) {
				if (fcntl(connectSd, F_SETFL, O_NONBLOCK) < 0) {
					printf("Failed to change the socket to non-blocking\n");
					close(connectSd);
				} else {
					clientSocketInfo.insert(std::make_pair(connectSd, std::string()));
				}
			}
		}
	}

/*
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
*/
	char buffer[10];
	int n;

	for (ClientSocketInfo::iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter) {
		if (FD_ISSET(iter->first, &readFds)) {
			if ((n=	recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT)) != 0) {
				buffer[n] = '\0';
				printf("receive - [%s]\n", buffer);
				iter->second.append(buffer);
			} 
		} else if ((FD_ISSET(iter->first, &readFds) == 0) && !iter->second.empty()) {
				printf("====EVENT: %d, %s, %ld\n", iter->first, iter->second.c_str(), strlen(iter->second.c_str()));
				printf("EOF\n");
				// send(iter->first, iter->second.c_str(), strlen(iter->second.c_str()), MSG_DONTWAIT);
				std::string eventMessage = iter->second;
				ConnectSd connectSd = iter->first;
				// close(iter->first);
				clientSocketInfo.erase(iter);
				
				return std::make_pair(connectSd, eventMessage);
		}
	}
	return std::make_pair(-1, Event());
}