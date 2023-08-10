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
		std::string filepath = rootPath + paramMap.at("requestURL");
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
			response.setStatus(200);
			return rootPath + "/" + model["index"];
		}else if (ifs.is_open()) {
			response.setStatus(200);
			return filepath;
		}
		response.setStatus(404);
		return model["404"];
    };

// 	Response staticWebpage(std::string filename) {
// 		std::string filename2 = filename;
// 		if (filename.size() >= 1 && filename[0] == '/')
// 			filename2 = filename.substr(1, filename.size() - 1);
// 		std::fstream fs(filename2.c_str());
		
// 		std::ostringstream buffer;
// 		buffer << fs.rdbuf();
// 		fs.close();
// 		return std::make_pair(this->code, buffer.str());
// 	};
};

class PostHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PostHandler(const ServerConfig &config): config(config) {

	};
    ~PostHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
		(void)paramMap;
		(void)model;
		(void)response;
		throw std::runtime_error("reached POST!!!");
    };
};
