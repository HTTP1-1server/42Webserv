#include "Multiplexer.hpp"

Event SelectMultiplexer::detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
    int maxSd = listenSd;
	//add child sockets to set 
	for (int i = 0 ; i < numClients; i++)
		if(clientSockets[i] > maxSd)
			maxSd = clientSockets[i];
	//clear the socket set 
	FD_ZERO(&readFds);  

	//add master socket to set 
	FD_SET(listenSd, &readFds);

	for (int i = 0; i < numClients; i++)
		FD_SET(clientSockets[i], &readFds);
	//wait for an eventCounts on one of the sockets , timeout is NULL , 
	//so wait indefinitely 
	if (select(maxSd + 1 , &readFds , NULL , NULL , NULL) < 0) {
		printf("select error\n");
	}
	if (FD_ISSET(listenSd, &readFds)) {
		int connectSd = accept(listenSd, sockAddr, sockAddrLen);
		if (connectSd >= 0) {
			if (fcntl(connectSd, F_SETFL, O_NONBLOCK) < 0) {
				printf("Failed to change the socket to non-blocking\n");
				close(connectSd);
			} else {
				clientSockets[numClients++] = connectSd;
			}
		}
	}

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
	char buffer[1000];
	int n;

	for (int i = 0; i < numClients; i++) {
		if (FD_ISSET(clientSockets[i], &readFds)) {
			if ((n=recv(clientSockets[i], buffer, sizeof(buffer), MSG_DONTWAIT))!=0) {
				buffer[n] = '\0';
				printf("receive - [%s]\n", buffer);
				sleep(2);
				send(clientSockets[i], message, strlen(message), MSG_DONTWAIT);
				// printf("send - [%s]\n", message);
			} else {
				printf("EOF\n");
				close(clientSockets[i]);
				if (i != numClients - 1)
					clientSockets[i] = clientSockets[numClients - 1];
				numClients--;
				continue;
			}
			// send(clientSockets[i], message, strlen(message), 0);
		}
	}


	
	// TODO: 클라이언트 너무 많이 요청오면 처리
	// if (numClients == MAX_CLIENTS)
	//     numClients %= MAX_CLIENTS;
    return Event();
}