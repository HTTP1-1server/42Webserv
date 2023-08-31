#pragma once

#include <map>
#include <string>
#include <vector>
#include "ServerConfig.hpp"

std::string getFullURL(std::vector<ServerConfig>::const_iterator serverConfig, const std::string &locationDir);

class Model: public std::map<std::string, std::string> {
private:
public:
    Model() {};
    ~Model() {};
    
    void add(const std::string &key, const std::string &value);
    void add(const std::string &key, HashMap::const_iterator locationIter, std::vector<ServerConfig>::const_iterator serverConfig);
    void addErrorPages(std::vector<ServerConfig>::const_iterator serverConfig);
};

std::map<std::string, Model> createModels(const std::vector<ServerConfig> &configs);