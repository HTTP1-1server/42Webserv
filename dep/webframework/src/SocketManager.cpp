#include "SocketManager.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

static std::string ooo;
static int count;

SocketDetail SelectSocketManager::getSocketDetail(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
    int maxSd = listenSd;

    for (SocketDetails::const_iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter)
        if (iter->first > maxSd)
            maxSd = iter->first;


    FD_ZERO(&readFds);
    FD_SET(listenSd, &readFds);	
	if (count >= 1) {
		(void)count;
	} else {
		for (SocketDetails::const_iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter)
			FD_SET(iter->first, &readFds);
	}

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

const char *message = "HTTP/1.1 200 OK\r\n\
Content-Length: 30\r\n\
Content-Location: /index.html\r\n\
Content-Type: text/html\r\n\
Date: Wed, 26 Jul 2023 09:12:46 GMT\r\n\
Last-Modified: Wed, 26 Jul 2023 07:27:09 GMT\r\n\
Server: Weebserv/1.0.0 (Unix)\r\n\
Transfer-Encoding: identity\r\n\
\r\n\
This is the default index yo!\n\0";

	// std::cout << "fdddd\n";
	for (SocketDetails::iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter) {
		// std::cout << "for 도는 중\n";
		if (FD_ISSET(iter->first, &readFds)) {
			memset(buffer, 0, sizeof(buffer));
			std::cout << sizeof(buffer) << std::endl;
			n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
			std::cout << n << ": " << buffer << std::endl;

			if (n < 1) {
				send(iter->first, message, strlen(message), MSG_DONTWAIT);
				count++;
				FD_CLR(iter->first, &readFds);
				close(iter->first);
				// clientSockets.erase(iter);
				break;
			}
			// if (n > 0) {
			// 	std::cout << n << std::endl;
			// } else if (n == -1) {
			// 	std::cout << n << 
			// }
		}
	}


    for (SocketDetails::iterator iter = this->clientSockets.begin(); iter != this->clientSockets.end(); ++iter) {
		// if (count > 1) {
		// 	break;
		// }
        if (FD_ISSET(iter->first, &readFds)) {
			n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
			buffer[n] = '\0';
			ooo.append(buffer);
			std::cout << ooo << std::endl;
			count++;

        //     if (n > 0) {
        //         buffer[n] = '\0';
        //         std::string &message = iter->second;
        //         message.append(std::string(buffer));
        //     } else if (n == 0) {
        //         int connectSd = iter->first;
        //         std::string requestMessage = iter->second;
        //         clientSockets.erase(iter);
        //         return std::make_pair(connectSd, requestMessage);
        //     } else if (n == -1) {
		// 		std::cout << "Hello~~~~~~!!!!!" << std::endl;
		// 		break;
		// 	}
        // } else if ((FD_ISSET(iter->first, &readFds) == 0) && !iter->second.empty()) {
        //     int connectSd = iter->first;
        //     std::string requestMessage = iter->second;
        //     clientSockets.erase(iter);
        //     return std::make_pair(connectSd, requestMessage);
        // } else {
        //     std::cout << "Socket: " << iter->first << " MESSAGE: " << iter->second << std::endl;
        //     int connectSd = iter->first;
        //     if (connectSd > 2) {
        //         n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
        //         // close(connectSd);
        //         clientSockets.erase(iter);
        //     }
        //     break;
		}
		// std::cout << count << std::endl;
		// if (count == 1) {
			std::cout  << "hahahahahah~~~~!~!~!~!~!~!~~!~~~~\n" << strlen(message) << std::endl;
			send(iter->first, message, strlen(message), MSG_DONTWAIT);
			count++;
			close(iter->first); // TODO: 클로즈 해야할까 아닐까?
            // clientSockets.erase(iter);
			break;
		// }
    }
	std::cout << count << std::endl;;
    return std::make_pair(-1, RequestMessage());
}

void SelectSocketManager::sendResponseMessage(int connectSd, ResponseMessage &responseMessage) {
	
	send(connectSd, responseMessage.c_str(), responseMessage.length(), MSG_DONTWAIT);
	close(connectSd);
};
