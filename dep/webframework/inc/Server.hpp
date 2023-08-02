#pragma once

#include "Types.hpp"

#include <sys/socket.h>
#include <exception>
#include <netinet/in.h> // htons
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <sys/types.h>
#include <unistd.h>
#include "ServerConfig.hpp"
#include "utils/Handler.hpp"
#include <vector>

class Server {
private:
public:
    const ServerConfig &config;
    ListenSd listenSd;
    SocketAddr sockAddr;
    int sockAddrLen;
    
    Server(const ServerConfig &config);
    ~Server();

    ListenSd getListenSd() const;

    SocketAddr & getAddress();

    int & getAddrLen();

    void initSocket(const ServerConfig &config);

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