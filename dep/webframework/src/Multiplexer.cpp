#include "Multiplexer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

std::pair<ConnectSd, RequestMessage> SelectMultiplexer::detectEvent(ListenSd listenSd, struct sockaddr *sockAddr, socklen_t *sockAddrLen)
{
    int maxSd = listenSd;

    for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
        if (iter->first > maxSd)
            maxSd = iter->first;

    FD_ZERO(&readFds);
    FD_SET(listenSd, &readFds);

    for (ClientSocketInfo::const_iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter)
        FD_SET(iter->first, &readFds);

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
                    clientSocketInfo.insert(std::make_pair(connectSd, RequestMessage()));
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
    char buffer[10000];
    int n;

    for (ClientSocketInfo::iterator iter = this->clientSocketInfo.begin(); iter != this->clientSocketInfo.end(); ++iter) {
        if (FD_ISSET(iter->first, &readFds)) {
            if ((n = recv(iter->first, buffer, sizeof(buffer) - 1, MSG_DONTWAIT)) != 0) {
                // printf("=== n = %d ===\n", n);
                buffer[n] = '\0';
                
                // 파일 만들기 테스트
                // std::time_t t = std::time(0);
                // std::stringstream ss;
                // ss << "public/new_file" << t << ".jpg";
                // std::fstream fs(ss.str(), std::ios::binary | std::ios::out);
                // fs.write(buffer, sizeof(buffer)); 	
                // fs.close();

                RequestMessage requestMessage(buffer, buffer + n);
                requestMessage.push_back('\0');
                // printf("receive - [%s]\n", requestMessage.data());
                iter->second.insert(iter->second.end(), requestMessage.begin(), requestMessage.end());

                /*
                message가 std::string일 때의 append()문

                iter->second.append(buffer);
                printf("iter length = %ld\n",  iter->second.length());
                */
            }
        } else if ((FD_ISSET(iter->first, &readFds) == 0) && !iter->second.empty()) {
            // printf("----- Request Message -----\n");
            // printf("\nConnect Socket: %d\n", iter->first);
            // printf("\nSize of the message: %ld\n", iter->second.size());
            // printf("\nThe Whole Request Message\n");
            // for (int i = 0; i < (int)iter->second.size(); ++i) printf("%c", iter->second[i]);
            // printf("---------------------------\n");

            /*
            message가 std::string일 때의 로그 코드

            printf("====EVENT: %d, %s, %ld\n", iter->first, iter->second.data(), iter->second.length());
            printf("EOF\n");

            message가 std::string일 때의 send문
            send(iter->first, iter->second.c_str(), strlen(iter->second.c_str()), MSG_DONTWAIT);
            */
            ConnectSd connectSd = iter->first;
            RequestMessage requestMessage = iter->second;
            requestMessage.push_back('\0');
            clientSocketInfo.erase(iter);
            return std::make_pair(connectSd, requestMessage);
        }
    }
    return std::make_pair(-1, RequestMessage());
}
