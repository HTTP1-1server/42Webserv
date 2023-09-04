#pragma once

#include <exception>
#include "ServerConfig.hpp"
#include "Types.hpp"

class Server {
private:
public:
    const ServerConfig &config;
    ListenSd listenSd;
    SocketAddrV6 sockAddr;
    int sockAddrLen;
    
    Server(const ServerConfig &config);
    ~Server();

    ListenSd getListenSd() const;

    SocketAddrV6 & getAddress();

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