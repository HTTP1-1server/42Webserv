#include "Multiplexer.hpp"

Event SelectMultiplexer::detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
	int maxSd = listenSd;
	//add child sockets to set
	for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
		if (iter->first > maxSd)
			maxSd = iter->first;
	// for (int i = 0 ; i < numClients; i++)
	// 	if(clientSockets[i] > maxSd)
	// 		maxSd = clientSockets[i];
	//clear the socket set 
	FD_ZERO(&readFds);  

	//add master socket to set 
	FD_SET(listenSd, &readFds);

	for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
		FD_SET(iter->first, &readFds);
	// for (int i = 0; i < numClients; i++)
	// 	FD_SET(clientSockets[i], &readFds);
	//wait for an eventCounts on one of the sockets , timeout is NULL , 
	//so wait indefinitely
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
					// clientSockets[numClients++] = connectSd;
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
	char buffer[1000];
	int n;

// RETURN VALUE         top
//    These calls return the number of bytes received, or -1 if an
//    error occurred.  In the event of an error, errno is set to
//    indicate the error.

//    When a stream socket peer has performed an orderly shutdown, the
//    return value will be 0 (the traditional "end-of-file" return).

//    Datagram sockets in various domains (e.g., the UNIX and Internet
//    domains) permit zero-length datagrams.  When such a datagram is
//    received, the return value is 0.

//    The value 0 may also be returned if the requested number of bytes
//    to receive from a stream socket was 0.

	for (ClientSocketInfo::iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter) {
		if (FD_ISSET(iter->first, &readFds)) {
			if ((n=	recv(iter->first, buffer, sizeof(buffer), MSG_DONTWAIT)) != 0) {
				buffer[n] = '\0';
				printf("receive - [%s]\n", buffer);
				// sleep(2);
				iter->second.append(buffer);
				// send(iter->first, iter->second.c_str(), strlen(iter->second.c_str()), MSG_DONTWAIT);				
				// printf("send - [%s]\n", message);
			} else {
				printf("====EVENT: %d, %s, %ld\n", iter->first, iter->second.c_str(), strlen(iter->second.c_str()));
				printf("EOF\n");
				send(iter->first, iter->second.c_str(), strlen(iter->second.c_str()), MSG_DONTWAIT);				
				std::string eventMessage = iter->second;
				close(iter->first);
				clientSocketInfo.erase(iter);
				
				return eventMessage;
				// if (i != numClients - 1)
				// 	clientSockets[i] = clientSockets[numClients - 1];
				// numClients--;
				// continue;
			}
		}
	}
	// for (int i = 0; i < numClients; i++) {
	// 	if (FD_ISSET(clientSockets[i], &readFds)) {
	// 		if ((n=recv(clientSockets[i], buffer, sizeof(buffer), MSG_DONTWAIT))!=0) {
	// 			buffer[n] = '\0';
	// 			printf("receive - [%s]\n", buffer);
	// 			// sleep(2);
	// 			send(clientSockets[i], message, strlen(message), MSG_DONTWAIT);
	// 			// printf("send - [%s]\n", message);
	// 		} else {
	// 			printf("EOF\n");
	// 			close(clientSockets[i]);
	// 			if (i != numClients - 1)
	// 				clientSockets[i] = clientSockets[numClients - 1];
	// 			numClients--;
	// 			continue;
	// 		}
	// 		// send(clientSockets[i], message, strlen(message), 0);
	// 	}
	// }


	
	// TODO: 클라이언트 너무 많이 요청오면 처리
	// if (numClients == MAX_CLIENTS)
	//     numClients %= MAX_CLIENTS;
	return Event();
}