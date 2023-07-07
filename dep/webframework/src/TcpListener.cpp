#include "TcpListener.hpp"
#include <unistd.h>

TcpListener::TcpListener(const Port port) {
    //create a master socket
    if((_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
        throw SocketCreationException();

    int opt = 1;
    //set master socket to allow multiple connections,
    //this is just a good habit, it will work without this
    if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
        sizeof(opt)) < 0)
        throw SocketOptionException();

    //type of socket created
    _address.sin_family = AF_INET;
    _address.sin_addr.s_addr = INADDR_ANY;
    _address.sin_port = htons(port);

    //bind the socket to localhost port
    if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address))<0)
        throw SocketBindingException();

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(_socket, 3) < 0)
        throw SocketListeningException();

    //accept the incoming connection
    _addrlen = sizeof(_address);
}

TcpListener::~TcpListener()
{
    if (_socket > 2)
        close(_socket);
}

template<>
TcpListener::Socket TcpListener::accept<Select>(void) {
	return 0;
};

int TcpListener::getSocket() const {
    return _socket;
}

TcpListener::SocketAddr & TcpListener::getAddress() {
    return _address;
}

int & TcpListener::getAddrLen() {
    return _addrlen;
}

const char *TcpListener::SocketCreationException::what() throw()
{
    return "cannot create socket";
}

const char *TcpListener::SocketOptionException::what() throw()
{
    return "cannot set socket option";
}

const char *TcpListener::SocketBindingException::what() throw()
{
    return "cannot bind socket";
}

const char *TcpListener::SocketListeningException::what() throw()
{
    return "cannot listen socket";
}
