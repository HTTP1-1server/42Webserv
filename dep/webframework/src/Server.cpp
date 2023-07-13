#include "Server.hpp"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

Server::Server(const Port port) {
	//create a master socket
	if ((listenSd = socket(AF_INET , SOCK_STREAM , 0)) == 0)
		throw SocketCreationException();
	if (fcntl(listenSd, F_SETFL, O_NONBLOCK) < 0)
		throw SocketOptionException();
	int opt = 1;
	//set master socket to allow multiple connections,
	//this is just a good habit, it will work without this
	if(setsockopt(listenSd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
		sizeof(opt)) < 0)
		throw SocketOptionException();

	//type of socket created
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;
	sockAddr.sin_port = htons(port);
	
	//bind the socket to localhost port
	if (bind(listenSd, (struct sockaddr *)&sockAddr, sizeof(sockAddr))<0)
		throw SocketBindingException();

	//try to specify maximum of 3 pending connections for the master socket
	if (listen(listenSd, 3) < 0)
		throw SocketListeningException();

	//accept the incoming connection
	sockAddrLen = sizeof(sockAddr);
	numClients = 0;
}

Server::~Server()
{
	// if (listenSd >= 0)
	//     close(listenSd);
}

template<>
int Server::acceptSdBy<Select>(void) {
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
		int connectSd = accept(listenSd, (struct sockaddr *)&sockAddr, (socklen_t *)&sockAddrLen);
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
				// printf("receive - [%s]\n", buffer);
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

	return -1;
}

ListenSd Server::getListenSd() const {
	return listenSd;
}

SocketAddr & Server::getAddress() {
	return sockAddr;
}

int & Server::getAddrLen() {
	return sockAddrLen;
}

const char* Server::SocketCreationException::what() const throw()
{
	return "cannot create socket";
}

const char* Server::SocketOptionException::what() const throw()
{
	return "cannot set socket option";
}

const char* Server::SocketBindingException::what() const throw()
{
	return "cannot bind socket";
}

const char* Server::SocketListeningException::what() const throw()
{
	return "cannot listen socket";
}

