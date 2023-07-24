#include "Server.hpp"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

Server::Server(const ServerConfig &config): config(config) {
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

	int port = *config.at("listen").data;
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

