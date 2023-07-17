#pragma once

#include <string>

typedef int         Code;
typedef std::string Body;

class ResponseMessage {
private:
	Code code;
	Body body;

public:
	std::string message;

	ResponseMessage(std::pair<Code, Body> pair) {
		const std::string PROTOCOL("HTTP/1.1 ");
		const std::string CODE_OK("200 ");
		const std::string STATUS_OK("OK\r\n");
		const std::string CONTENT_TYPE("Content-Type: text/html\r\n");

		std::string header = PROTOCOL + CODE_OK + STATUS_OK + CONTENT_TYPE;
		// header = list.join();
		message = header + std::string("\n") + pair.second;
	};
	~ResponseMessage() {};
};

/*String request message -> Event -> handler -> processRequest(body, code) -> Response Message

요청메시지

// GET(static), GET(image), GET(CGI), POST(image), POST(CGI), DELETE(image)

/* == GET(Static) Success ==
HTTP/1.1 200 OK\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: 53\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!index.html-->"
*/

/* == GET(Static) Fail ==
HTTP/1.1 404 Error\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!error_404.html-->"
*/

/* == GET(image) Success ==
HTTP/1.1 200 OK\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!image.html-->"
*/

/* == GET(image) 요청URL에 이미지 없는 경우 Fail ==
HTTP/1.1 404 Error\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!error_404.html-->"
*/

/* == POST(image) 잘못된 이미지 업로드 Fail ==
HTTP/1.1 400 Error\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!error_400.html-->"
*/

// TODO: 
/* == GET(cgi) Success ==
HTTP/1.1 200 OK\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!image.html-->"
*/

// TODO: 
/* == GET(cgi) Failed ==
HTTP/1.1 200 OK\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!image.html-->"
*/

/* == DELETE(image) Success ==
HTTP/1.1 200 OK\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
"
*/

/* == DELETE(image) Fail ==
HTTP/1.1 400 Error\r\n
Server: Apache/2.2.14 (Win32)\r\n
Content-Length: ??\r\n
Content-Type: text/html\r\n
Connection: close\r\n
\n
<--!error_400.html-->"
*/

// 응답메시지

// Success
// [Header]
// HTTPVersion "HTTP/1.1"
// Code "200"
// Status "OK"
// ConLength 53

// HTTP/1.1 200 OK
// Content-Length: 53\r\n\
// Content-Type: text/html\r\n\
// Connection: close\r\n\

// [Body]
// String


// 시작라인
// 	200 ...
// 	OK
// 	.
// 헤더
// 	.
// 	.
// 바디
// 	.


// */
