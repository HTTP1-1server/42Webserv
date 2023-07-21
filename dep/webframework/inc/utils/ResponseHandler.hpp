#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include "Multiplexer.hpp"
#include "RequestInfo.hpp"
#include <vector>
#include <algorithm>
#include <iostream>


typedef std::string Event;
typedef std::string Body;

class HttpResponseHandler;

class ResponseHandler {
protected:
    ResponseHandler() {};
public:
    virtual ~ResponseHandler() {};
    virtual std::pair<Code, Body> processResponse(RequestInfo requestMessage) = 0;
};

class HttpResponseHandler: public virtual ResponseHandler {
private:
public:
	std::string db;
	std::string fileName;
	int code;

    HttpResponseHandler() {};
    ~HttpResponseHandler() {};
    std::pair<Code, Body> processResponse(RequestInfo requestMessage) {
		std::string requestURL = requestMessage.url;
		std::string requestMethod = requestMessage.method;

		std::vector<char> requstBody = requestMessage.dody;
		std::string startBoundary = requestMessage.boundary;
		std::string endBoundary = "--" + startBoundary + "--";


		std::vector<char> removedBoundary = removeBoundary(requstBody, startBoundary);
		std::vector<char> perfectImage = removeTrailingBoundary(removedBoundary, endBoundary);
		std::cout << perfectImage.size() << std::endl;

		if (startBoundary.length() != 0) {
			sout("startBoundary = " + startBoundary);
			sout("endBoundary = " + endBoundary);
			std::cout << "+++++++++++++++++++++++++++++++++start++++++++++++++++++++++++++++++++++++" << std::endl;
			for (std::vector<char>::const_iterator iter = requstBody.begin(); iter != requstBody.end(); ++iter) {
				std::cout << *iter;
			}
			std::cout << "++++++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++" << std::endl;
		}

		if (startBoundary.length() != 0) {
			sout("startBoundary = " + startBoundary);
			sout("endBoundary = " + endBoundary);
			std::cout << "+++++++++++++++++++++++++++++++++start++++++++++++++++++++++++++++++++++++" << std::endl;
			for (std::vector<char>::const_iterator iter = perfectImage.begin(); iter != perfectImage.end(); ++iter) {
				std::cout << *iter;
			}
			std::cout << "++++++++++++++++++++++++++++++++end+++++++++++++++++++++++++++++++++++++" << std::endl;
		}

        std::time_t t2 = std::time(0);
        std::stringstream sss;
        sss << "public/perfectImage" << t2 << ".png";

		// std::fstream fs(sss.str().c_str(), std::ios::binary | std::ios::out);
        int fd = open(sss.str().c_str(), O_CREAT | O_WRONLY, 777);
        write(fd, perfectImage.data(), perfectImage.size());
		// fs.write(perfectImage.data(), perfectImage.size());
        close(fd);
		// fs.close();

		// sout("");
		// sout("Request URL = " + requestURL);
		// sout("Request Method = " + requestMethod);
		// sout("Request Body\n%s" + requstBody);
		// sout("--------------------------------------end of body\n\n\n\n");
		// sout("");

		db = "public";
		// chooseMethod(requestMessage);
        // 0. parse message -> memberValue

        // std::map<"key", &Fn> fnTable;
        // fnTable[("GET", "/")] = &staticWebpate;

        // 1. method check 
        //  GET(static) -> 
        //  GET(image)
        //  GET(CGI)
        //  POST(image)
        //  POST(CGI)
        //  DELETE(image)

        // 2. method exec
		if (requestMessage.url == "/") {
			fileName = db + "/index.html";
		}
		// sout("fileName is " + fileName);
        return staticWebpage(fileName);
    };

	std::pair<Code, Body> staticWebpage(std::string filename) {
		std::string filename2 = filename;
		if (filename.size() >= 1 && filename[0] == '/')
			filename2 = filename.substr(1, filename.size() - 1);
		std::fstream fs(filename2.c_str());
		std::stringstream buffer;
		buffer << fs.rdbuf();
		fs.close();
		return std::make_pair(code, buffer.str());
	};

	// void chooseMethod(RequestInfo requestMessage) {
	// 	fileName = db + requestMessage.url;

	// 	if (requestMessage.method == "GET") methodGET();
	// 	else if (requestMessage.method == "POST") methodPOST(requestMessage);
	// }

	// void methodGET() {
	// 	if (fileExists(fileName)) code = 200;
	// 	else {
	// 		code = 404;
	// 		fileName = db + "/404_error.html";
	// 	}
	// }

	// void methodPOST(RequestInfo requestMessage) {
	// 	if (requestMessage.url =="/upload" && isFileEmpty(requestMessage)) {
	// 		fileName = db + "/index.html";
	// 		code = 200;
	// 	} else if (requestMessage.url == "/upload") {
	// 		code = 302;
	// 		// std::string fileData = parseMultipart(requestMessage.body);
	// 		std::string fileData = requestMessage.body;
	// 		saveFile(fileData, db + makeRandomName());
	// 		fileName = db + "/index.html";
	// 	}
	// }

	// bool isFileEmpty(RequestInfo requestMessage) {
	// 	std::string msg = requestMessage.body;
	// 	for (int i = 0; i < msg.length(); i++) {
	// 		if (msg[i] == 'f'
	// 			&& msg[i + 1] == 'i'
	// 			&& msg[i + 2] == 'l'
	// 			&& msg[i + 3] == 'e'
	// 			&& msg[i + 4] == 'n'
	// 			&& msg[i + 5] == 'a'
	// 			&& msg[i + 6] == 'm'
	// 			&& msg[i + 7] == 'e'
	// 			&& msg[i + 8] == '='
	// 			) {
	// 				if (msg[i + 9] == '"' && msg[i + 10] == '"') {
	// 					return true;
	// 				}
	// 				return false;
	// 			}
	// 	}
	// 	return false;
	// }

	std::string parseMultipart(std::string input) {
		std::string boundary = input.substr(0, input.find("\r\n"));
		size_t start = input.find(boundary) + boundary.length() + 2;
		size_t end = input.find(boundary, start);
		return input.substr(start, end - start);
	}

	std::string makeRandomName() {
		std::time_t t = std::time(0);
        std::stringstream ss;
        ss << "/uploaded_image_" << t << ".jpg";
		return ss.str();
	}

	void saveFile(std::string fileData, std::string filePath) {
		std::ofstream outFile(filePath.c_str(), std::ios::binary);
		if (outFile.is_open()) {
			outFile << fileData;
			outFile.close();
			std::cout << "File has been saved to " << filePath << std::endl;
		} else {
			std::cout << "Failed to open file for writing: " << filePath << std::endl;
		}
	}

	bool fileExists(std::string fileName)
	{
		std::ifstream infile(fileName);
		return infile.good();
	}

	void sout(std::string input) {
		std::cout << input << std::endl;
	}

	std::vector<char> removeBoundary(const std::vector<char>& inputData, const std::string& boundary) {
		std::vector<char> result;

		// Convert the boundary string to bytes
		std::vector<char> boundaryBytes;
		for (std::size_t i = 0; i < boundary.size(); ++i) {
			boundaryBytes.push_back(boundary[i]);
		}

		// Find the position of the boundary in the input data
		std::vector<char>::const_iterator startIt = inputData.begin();
		while (true) {
			startIt = std::search(startIt, inputData.end(), boundaryBytes.begin(), boundaryBytes.end());
			if (startIt == inputData.end()) {
				break; // Boundary not found, exit loop
			}

			// Calculate the position where the headers end and image data starts
			std::vector<char>::const_iterator imageDataStart = startIt + boundaryBytes.size();
			bool headersFound = false;
			while (imageDataStart != inputData.end()) {
				if (*imageDataStart == '\r' && imageDataStart + 3 != inputData.end() &&
					*(imageDataStart + 1) == '\n' && *(imageDataStart + 2) == '\r' && *(imageDataStart + 3) == '\n') {
					headersFound = true;
					break;
				}
				++imageDataStart;
			}

			if (!headersFound) {
				break; // Headers not found, exit loop
			}

			// Add the image data after the headers
			result.insert(result.end(), imageDataStart + 4, inputData.end());

			// Move the iterator to search for the next boundary
			startIt = imageDataStart + 4;
		}

		return result;
	}

	std::vector<char> removeTrailingBoundary(const std::vector<char>& inputData, const std::string& boundary) {
		std::vector<char> result = inputData;

		// Convert the boundary string to bytes
		std::vector<char> boundaryBytes;
		for (std::size_t i = 0; i < boundary.size(); ++i) {
			boundaryBytes.push_back(boundary[i]);
		}

		// Find the position of the trailing boundary in the result data
		std::vector<char>::const_iterator trailingBoundaryStart = std::search(result.begin(), result.end(), boundaryBytes.begin(), boundaryBytes.end());
		if (trailingBoundaryStart != result.end()) {
			// Erase everything after the trailing boundary
			result.erase(trailingBoundaryStart, result.end());
		}

		return result;
	}

};

ResponseHandler *generate(std::string protol) {
    if (protol == "HTTP")
        return new HttpResponseHandler();
    return 0;
    // if (protol == "HTTPS")
    //     return new HttpsResponseHandler();
}