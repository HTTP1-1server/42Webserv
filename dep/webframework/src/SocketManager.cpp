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
                if (fcntl(connectSd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0) {
                    printf("Failed to change the socket to non-blocking\n");
                    close(connectSd);
                } else {
                    clientSockets.insert(std::make_pair(connectSd, RequestMessage()));
                }
            }
        }
    }

    char buffer[1000000];
    int n;

    for (SocketDetails::iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter) {
        if (FD_ISSET(iter->first, &readFds)) {
			n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
			buffer[n] = '\0';
            if (n == 0)
                continue;
            // if (n < 0)
            //     throw std::runtime_error("what");
            iter->second.append(buffer);
            SocketDetail socketDetail = std::make_pair(iter->first, iter->second);
            clientSockets.erase(iter);
            return socketDetail;
		}
    }
    return std::make_pair(-1, RequestMessage());
}

void SelectSocketManager::sendResponseMessage(int connectSd, const std::string &responseMessage) {
    // std::cout << "++RESPONSE++\n" << responseMessage << std::endl;
    const char *msg = responseMessage.c_str();
    int msgLength = responseMessage.length();
    int index = 0;
    int msgSended = send(connectSd, msg + index, msgLength, 0);
	while (msgSended > 0) {
        index += msgSended;
        msgLength -= msgSended;
        // if (msgLength < 0)
        //     break;
        msgSended = send(connectSd, msg + index, msgLength, 0);
		std::cout << "connectSd = " << connectSd << std::endl;
		std::cout << "msg = " << msg << std::endl;
		std::cout << "index = " << index << std::endl;
		std::cout << "msgLength = " << msgLength << std::endl;
		std::cout << "msgSended = " << msgSended << std::endl;
		std::cout << "----------------------" << std::endl;
    }
	std::cout << "Return of index() = " << index + msgLength << std::endl;
	close(connectSd);
    // std::cout << "++SOCKET CLOSED++\n" << connectSd << std::endl;
};
