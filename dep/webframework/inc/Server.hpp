#pragma once

#include <sys/socket.h>
#include <exception>
#include <netinet/in.h> // htons
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS (30)

enum IoMultiplex {
    Select,
    Epoll,
    Kqueue,
};

typedef int                 Port;
typedef int                 ListenSd;
typedef struct sockaddr_in  SocketAddr;

class Server {
private:
    fd_set readFds;
    int clientSockets[MAX_CLIENTS];
    int numClients;

public:
    ListenSd listenSd;
    SocketAddr sockAddr;
    int sockAddrLen;
    
    Server(const Port port);
    ~Server();

    ListenSd getListenSd() const;

    SocketAddr & getAddress();

    int & getAddrLen();

    class SocketCreationException : public std::exception {
		public:
			virtual const char* what() const throw();
    };
    class SocketOptionException : public std::exception {
        public:
			virtual const char* what() const throw();
    };
    class SocketBindingException : public std::exception {
        public:
			virtual const char* what() const throw();
    };
    class SocketListeningException : public std::exception {
        public:
			virtual const char* what() const throw();
    };
};