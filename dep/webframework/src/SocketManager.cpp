#include "SocketManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

SocketDetail SelectSocketManager::getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
    int maxSd = listenSd;

    for (SocketDetails::const_iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter)
        if (iter->first > maxSd)
            maxSd = iter->first;


    FD_ZERO(&readFds);
    FD_SET(listenSd, &readFds);	
	for (SocketDetails::const_iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter)
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
                    clientSockets.insert(std::make_pair(connectSd, RequestMessage()));
                }
            }
        }
    }

    char buffer[100];
    int n;

    for (SocketDetails::iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter) {
        if (FD_ISSET(iter->first, &readFds)) {
			n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
			buffer[n] = '\0';
			iter->second.append(buffer);
            SocketDetail socketDetail = std::make_pair(iter->first, iter->second);
            clientSockets.erase(iter);
            return socketDetail;
		}
    }
    return std::make_pair(-1, RequestMessage());
}

void SelectSocketManager::sendResponseMessage(int connectSd, ServletResponse &responseMessage) {
    std::cout << "++RESPONSE++\n" << responseMessage << std::endl;
	send(connectSd, responseMessage.c_str(), responseMessage.length(), MSG_DONTWAIT);
	close(connectSd);
};
