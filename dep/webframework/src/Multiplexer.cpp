#include "Multiplexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

std::pair<ConnectSd, RequestMessage> SelectMultiplexer::detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
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
                    clientSocketInfo.insert(std::make_pair(connectSd, RequestMessage()));
                }
            }
        }
    }

    char buffer[10];
    int n;

    for (ClientSocketInfo::iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter) {
        if (FD_ISSET(iter->first, &readFds)) {
            if ((n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT)) != 0) {
                buffer[n] = '\0';
                std::string &message = iter->second;
                message.append(std::string(buffer));
            }
        } else if ((FD_ISSET(iter->first, &readFds) == 0) && !iter->second.empty()) {
            ConnectSd connectSd = iter->first;
            RequestMessage requestMessage = iter->second;
            requestMessage.push_back('\0');
            clientSocketInfo.erase(iter);
            return std::make_pair(connectSd, requestMessage);
        }
    }
    return std::make_pair(-1, RequestMessage());
}
