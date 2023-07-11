#include "Server.hpp"
#include <stdio.h>
#include <errno.h>

Server::Server(const Port port) {
    //create a master socket
    if((listenSd = socket(AF_INET , SOCK_STREAM , 0)) == 0)
        throw SocketCreationException();

    int opt = 1;
    //set master socket to allow multiple connections,
    //this is just a good habit, it will work without this
    if(setsockopt(listenSd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
        sizeof(opt)) < 0)
        throw SocketOptionException();

    //type of socket created
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
}

Server::~Server()
{
    if (listenSd > 2)
        close(listenSd);
}

template<>
int Server::acceptSdBy<Select>(void) {
    int activity, sd, maxSd, maxClients = 30;
    //clear the socket set 
    FD_ZERO(&readFds);  

    //add master socket to set 
    FD_SET(listenSd, &readFds);  
    maxSd = listenSd;  
        
    //add child sockets to set 
    for (int i = 0 ; i < maxClients; i++)  
    {  
        //socket descriptor 
        sd = clientSockets[i];  
            
        //if valid socket descriptor then add to read list 
        if(sd > 0)  
            FD_SET(sd, &readFds);  
            
        //highest file descriptor number, need it for the select function 
        if(sd > maxSd)  
            maxSd = sd;  
    }  

    //wait for an activity on one of the sockets , timeout is NULL , 
    //so wait indefinitely 
    activity = select( maxSd + 1 , &readFds , NULL , NULL , NULL);  

    if ((activity < 0) && (errno!=EINTR)) {
        printf("select error");  
    }  
        
    if (FD_ISSET(listenSd, &readFds)) return accept(listenSd, (struct sockaddr *)&sockAddr, (socklen_t*)&sockAddrLen);
	return Select;
};

int Server::getListenSd() const {
    return listenSd;
}

SocketAddr & Server::getAddress() {
    return sockAddr;
}

int & Server::getAddrLen() {
    return sockAddrLen;
}

const char *Server::SocketCreationException::what() throw()
{
    return "cannot create socket";
}

const char *Server::SocketOptionException::what() throw()
{
    return "cannot set socket option";
}

const char *Server::SocketBindingException::what() throw()
{
    return "cannot bind socket";
}

const char *Server::SocketListeningException::what() throw()
{
    return "cannot listen socket";
}
