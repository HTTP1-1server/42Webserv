#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 8080

int main(int argc, char *argv[])
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len;

	int n;
	char buffer[BUFSIZ];

	s_socket = socket(PF_INET, SOCK_STREAM, 0);

	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_port = htons(PORT);

	if (bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1) {
		printf("Can not Bind\n");
		return -1;
	}
	if (listen(s_socket, 5) == -1) {
		printf("listen Fail\n");
		return -1;
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
	
	fd_set readFds;
	int connectSd, sd, maxSd, maxClients = 30, listenSd, numClient;
	int clientSockets[maxClients];
	while (1) {
		int maxSd = listenSd;
		for (int i = 0; i < maxClients; i++)
		{
			if (clientSockets[i] > maxSd)
				maxSd = clientSockets[i];
		}
		maxSd++;

		FD_ZERO(&readFds); // readFds [ 0 0 0 0 0 ... ]
		FD_SET(listenSd, &readFds); // readFds [ 0 0 0 1 0 ... ]

		for (int i = 0 ; i < maxClients; i++)  
			FD_SET(clientSockets[i], &readFds); /* 연결된 후) 연결된 소켓을 read 이벤트 받을 수 있도록 셋 readFds [ 0 0 0 1 1 ... ]*/ 


		connectSd = select( maxSd + 1 , &readFds , NULL , NULL , NULL); // READ 이벤트가 발생할 때 까지 대기 
		/*새로운 연결이 왔으면 [ 0 0 0 1 0 0 ... ], 연결된 소켓에 데이터가 온 경우 readFds [ 0 0 0 0 1 0 ... ], 둘 다 오면 readFds [ 0 0 0 1 1 0 ... ]*/ 
		if (connectSd < 0) {
			printf("select error\n");
			// exit(-1);
		}
		printf("Waiting---\n"); // 사건 발생

		// 듣기 소켓
		if (FD_ISSET(listenSd, &readFds)) { // listenSd에서 깨어 났는지 확인
			connectSd =  accept(listenSd, (struct sockaddr *)&c_addr, (socklen_t*)&len);
			clientSockets[numClient++] = connectSd; // 연결 소켓 목록 저장 [ 4 0 0 0 0 ... ]
													/* [ 4 5 0 0 0 ... ] */
		}

		// 연결 소켓
		for (int i = 0; i < maxClients; i++) {
			if (FD_ISSET(clientSockets[i], &readFds)) { // 연결 소켓에서 read 이벤트가 있었는지 확인. 4 는 현재 바로 연결된 상태이므로 readFds [ 0 0 0 1 0 ... ] 0
				if ((n = read(clientSockets[i], buffer, sizeof(buffer)))!=0) {
					buffer[n] = '\n';
					printf("receive - [%s]\n", buffer);
					write(clientSockets[i], buffer, n); // echo
					printf("receive - [%s]\n", buffer);
				} else {
					printf("EOF\n");
					close(clientSockets[i]);
					if (i != maxClients - 1)
						clientSockets[i] = clientSockets[maxClients - 1];
					maxClients--;
					continue;
				}
			}
		}
	}
}
