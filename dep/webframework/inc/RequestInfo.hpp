#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include "Multiplexer.hpp"

typedef std::string Method;
typedef std::string Path;
typedef std::vector<char> RequestBody;

int cook(std::istream &stream) {
    while (!stream.fail()) {
        std::string header;
        std::getline(stream, header);
        if (header.length() < 1) {
            stream.clear();
            return stream.tellg();
        }
        if (header[header.size() - 1] == '\r') {
            continue;
        } else {
            stream.clear();
            return stream.tellg();
        }
    }
    return stream.tellg();
};

class RequestInfo
{
private:
    RequestInfo();

public:
    Method method;
    Path location;
	std::string boundary;
    RequestBody requestBody;
    Path url;

    RequestInfo(RequestMessage requestMessage) {
		std::string startBoundary;
		std::string endBoundary;

		requestMessage.push_back('\0');
		std::stringstream sstream(&requestMessage[0]);
        std::string token;

        sstream >> token; // GET
        this->method = token;

        sstream >> token; // url
        this->url = token;
		

        int pos = cook(sstream);
        std::vector<char>::iterator start = requestMessage.begin() + pos;
        std::vector<char>::iterator end = requestMessage.end();
		std::vector<char> requestBody(start, end);
		
		startBoundary = extractBoundary(requestMessage);
		endBoundary = "--" + startBoundary + "--";

		std::vector<char> removedBoundary = removeStartBoundary(requestBody, startBoundary);
		std::vector<char> perfectImage = removeEndBoundary(removedBoundary, endBoundary);
		this->requestBody = perfectImage;
    };
    ~RequestInfo() {};

	std::string extractBoundary(const std::vector<char>& requestData) {
		std::string boundaryStr;
		std::string contentTypeHeader = "Content-Type: multipart/form-data; boundary=";

		// Convert the request data vector to a string for easier processing
		std::string requestDataStr(requestData.begin(), requestData.end());

		// Find the position of the Content-Type header
		size_t contentTypePos = requestDataStr.find(contentTypeHeader);

		if (contentTypePos != std::string::npos) {
			// Extract the boundary value from the Content-Type header
			size_t boundaryStart = contentTypePos + contentTypeHeader.length();
			size_t boundaryEnd = requestDataStr.find("\r\n", boundaryStart);

			if (boundaryEnd != std::string::npos) {
				boundaryStr = requestDataStr.substr(boundaryStart, boundaryEnd - boundaryStart);
			}
		}

		return boundaryStr;
	}

	std::vector<char> removeStartBoundary(const std::vector<char>& inputData, const std::string& boundary) {
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


	std::vector<char> removeEndBoundary(const std::vector<char>& inputData, const std::string& boundary) {
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