#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>
#include "SocketManager.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "Model.hpp"

class Handler {
protected:
public:
    Handler() {};
    virtual ~Handler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const = 0;

	std::map<std::string, HashMap>::const_iterator findLocationBlock(const std::string &url, const std::map<std::string, HashMap> &locationBlocks) const {
		std::map<std::string, HashMap>::const_iterator location = locationBlocks.begin();

        for (std::map<std::string, HashMap>::const_iterator locationIter = locationBlocks.begin();
            locationIter != locationBlocks.end();
            ++locationIter
        ) {
            if (url.find(locationIter->first) != std::string::npos) {
                if (locationIter->first.length() >= location->first.length()) {
                    location = locationIter;
                }
            }
        }
        return location;
	}
};

class GetHandler: public virtual Handler {
private:
public:
	const HashMap &config;

    GetHandler(const HashMap &config): config(config) {

	};
    virtual ~GetHandler() {};
    std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
		std::string rootPath = "." + model["root"];
		std::string filepath = rootPath + paramMap.at("restOfRequest");
		std::ifstream ifs(filepath.c_str());
		
		std::cout << "FILEPATH: " << filepath << std::endl;
		DIR *dir;
		if ((dir = opendir(filepath.c_str()))) {
			if (model.find("autoindex") != model.end()) {
				if (model["autoindex"] == "on") {
					response.setStatus(200);
					return "./public/resources/autoIndex.html";
				}
			}
			closedir(dir);
			std::string innerIndexFileName = filepath + "/" + model["index"];
			std::ifstream innerIndex(innerIndexFileName.c_str());
			if (innerIndex.good()) {
				response.setStatus(200);
				return filepath + "/" + model["index"];
			}
		}else if (ifs.is_open()) {
			response.setStatus(200);
			return filepath;
		}
		response.setStatus(404);
		return model["404"];
    };
};

class PostHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PostHandler(const ServerConfig &config): config(config) {

	};
    ~PostHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
		if (paramMap.find("Content-Length") != paramMap.end() && paramMap.at("Content-Length") == "0") {
			response.setStatus(400);
			return model["400"];
		}
		
		if (model.find("client_max_body_size") != model.end()) {
			std::stringstream ss;
			ss << paramMap.at("bodyLength");
			int requestLength;
			ss >> requestLength;

			std::stringstream ss2;
			ss2 << model.at("client_max_body_size");
			int limit;
			ss2 >> limit;

			if (requestLength > limit) {
				response.setStatus(413);
				return model["413"];
			}
		}

		if (paramMap.at("body").empty()) {
			int statusCode = 400; // 테스터 돌릴때 두번째 테스트 POST empty body 405가 정답
			std::stringstream ss;
			std::string errorCode;
			ss << statusCode;
			ss >> errorCode;
			response.setStatus(statusCode);
			return model[errorCode];
		}
		return "." + model["root"] + "/" + model["index"];
    };
};
