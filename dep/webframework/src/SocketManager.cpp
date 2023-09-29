#include "SocketManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>


SocketDetail SelectSocketManager::getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
    int maxSd = listenSd;

    for (SocketDetails::const_iterator iter = this->recvSockets.begin(); iter != this->recvSockets.end(); ++iter)
        if (iter->first > maxSd)
            maxSd = iter->first;


    FD_ZERO(&readFds);
    FD_SET(listenSd, &readFds);
	for (SocketDetails::const_iterator iter = this->recvSockets.begin(); iter != this->recvSockets.end(); ++iter)
		FD_SET(iter->first, &readFds);

    struct timeval tv = {0, 5000};
    int activity = select(maxSd + 1 , &readFds , NULL , NULL , &tv);
    if (activity > 0) {
        if (FD_ISSET(listenSd, &readFds)) {
            int connectSd = accept(listenSd, sockAddr, sockAddrLen);
            if (connectSd >= 0) {
                if (fcntl(connectSd, F_SETFL, O_NONBLOCK, FD_CLOEXEC) < 0) {
                    printf("Failed to change the socket to non-blocking\n");
                    close(connectSd);
                } else {
                    recvSockets.insert(std::make_pair(connectSd, RequestMessage()));
                }
            }
        }
	} else {
    }

    char buffer[1000000];
    int n;
    for (SocketDetails::iterator iter = this->recvSockets.begin(); iter != this->recvSockets.end();) {
        if (FD_ISSET(iter->first, &readFds)) {
			n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
			buffer[n] = '\0';
            if (n == 0) {
                ++iter;
                continue;
            } else if (n < 0) {
                SocketDetails::iterator copy = iter;
                close(copy->first);
                ++iter;
                this->recvSockets.erase(copy);
                continue;
            }
            iter->second.append(buffer);
            SocketDetail socketDetail = std::make_pair(iter->first, iter->second);
            recvSockets.erase(iter);
            return socketDetail;
		}
        ++iter;
    }

    return std::make_pair(-1, RequestMessage());
}

void SelectSocketManager::addResponseMessage(int connectSd, const std::string &responseMessage) {
    this->sendSockets.insert(std::make_pair(connectSd, responseMessage));
};

void SelectSocketManager::sendResponseMessage() {
    int n;
    for (SocketDetails::iterator iter = this->sendSockets.begin(); iter != this->sendSockets.end();) {
        std::string &msg = iter->second;
        if (msg.length() == 0) {
            SocketDetails::iterator copy = iter;
            close(copy->first);
            ++iter;
            this->sendSockets.erase(copy);
            continue;
        }
        n = send(iter->first, msg.c_str(), msg.length(), 0);
        if (n > 0) {
            msg = msg.substr(n, msg.length() - n);
        } else if (n == 0) {
            ++iter;
            send(iter->first, msg.c_str(), msg.length(), 0);
            continue;
        } else {
            SocketDetails::iterator copy = iter;
            close(copy->first);
            ++iter;
            this->sendSockets.erase(copy);
            continue;
        }
        ++iter;
    }
}
