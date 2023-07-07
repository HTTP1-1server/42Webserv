#pragma once

#include <sys/socket.h>
#include <exception>
#include <netinet/in.h> // htons

enum IoMultiplex {
    Select,
    Epoll,
    Kqueue,
};

class TcpListener {
public:
    /**
     * @typedef Port number
     */
    typedef int                 Port;

    /**
     * @typedef Socket file descriptor
     */
    typedef int                 Socket;

    /**
     * @typedef Socket address
     */
    typedef struct sockaddr_in  SocketAddr;

    /**
     * @brief Construct a new Tcp Listener object
     * 
     * @param port port number
     */
    TcpListener(const Port port);

    /**
     * @brief Destroy the Tcp Listener object
     */
    ~TcpListener();

    /**
     * @brief Accept
     * 
     * @tparam T 
     * @return Socket 
     */
    template<IoMultiplex M>
    Socket accept(void);

    /**
     * @brief Get the Socket object
     * 
     * @return Socket 
     */
    Socket getSocket() const;

    /**
     * @brief Get the Address object
     * 
     * @return SocketAddr& 
     */
    SocketAddr & getAddress();

    /**
     * @brief Get the Addr Len object
     * 
     * @return int& 
     */
    int & getAddrLen();

    class SocketCreationException : public std::exception {
        virtual const char* what() throw();
    };
    class SocketOptionException : public std::exception {
        virtual const char* what() throw();
    };
    class SocketBindingException : public std::exception {
        virtual const char* what() throw();
    };
    class SocketListeningException : public std::exception {
        virtual const char* what() throw();
    };

private:
    Socket _socket;
    SocketAddr _address;
    int _addrlen;
};