#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include "SocketManager.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include <vector>
#include <algorithm>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

class Handler {
protected:
public:
    Handler() {};
    virtual ~Handler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, const std::map<std::string, HashMap> &locationBlocks) const = 0;

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
    std::string process(const std::map<std::string, std::string> &paramMap, const std::map<std::string, HashMap> &locationBlocks) const {
		std::map<std::string, HashMap>::const_iterator location = this->findLocationBlock(paramMap.at("fullURL"), locationBlocks);
		HashMap locationConfig = location->second;
	
		std::map<int, std::string> errorPages = *locationConfig.at("error_page").data;
		std::string rootPath = *locationConfig.at("root").data;
		rootPath = "." + rootPath;
		std::string filepath = rootPath + paramMap.at("requestURL");
		std::ifstream ifs(filepath.c_str());
		
		std::cout << "FILEPATH: " << filepath << std::endl;
		if (opendir(filepath.c_str())) {
			if (locationConfig.find("autoindex") != locationConfig.end()) {
				std::string autoindex = *locationConfig.at("autoindex").data;
				if (autoindex == "on") {
					return rootPath + "/resources/autoIndex.html";
				}
			}
			std::string indexPath = *locationConfig.at("index").data;
			return rootPath + "/" + indexPath;
		}else if (ifs.is_open()) {
			return filepath;
		}
		return errorPages.at(404);
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
    virtual std::string process(const std::map<std::string, std::string> &paramMap, const std::map<std::string, HashMap> &locationBlocks) const {
		(void)paramMap;
		(void)locationBlocks;
		throw std::runtime_error("reached POST!!!");
    };
};
