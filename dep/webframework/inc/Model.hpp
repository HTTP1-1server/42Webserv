#pragma once

#include <map>
#include <string>
#include <vector>
#include "ServerConfig.hpp"

inline std::string getFullURL(std::vector<ServerConfig>::const_iterator serverConfig, const std::string &locationDir) {
    std::string host = *serverConfig->at("host").data;
    int port = *serverConfig->at("listen").data;
    std::stringstream ssPort;
    ssPort << port;
    std::string portString;
    ssPort >> portString;
    return host + ":" + portString + locationDir;
};

class Model: public std::map<std::string, std::string> {
private:
public:
    Model() {};
    ~Model() {};
    
    void add(const std::string &key, const std::string &value) {
        this->insert(std::make_pair(key, value));
    }
    void add(const std::string &key, HashMap::const_iterator locationIter, std::vector<ServerConfig>::const_iterator serverConfig) {
        HashMap locationConfig = *locationIter->second.data;

        const HashMap *config;
        if (locationConfig.find(key) != locationConfig.end()){
            config = &locationConfig;
        } else if (serverConfig->find(key) != serverConfig->end()) {
            config = dynamic_cast<const HashMap *>(&*serverConfig);
        } else {
            return;
        }
        std::string value;
        try {
            std::string stringValue = *config->at(key).data;
            value = stringValue;
        } catch(const std::exception& e) {
            int intValue = *config->at(key).data;
            std::stringstream ss;
            ss << intValue;
            std::string stringValue;
            ss >> stringValue;
            value = stringValue;
        }
        this->insert(std::make_pair(key, value));
    }
    void addErrorPages(std::vector<ServerConfig>::const_iterator serverConfig) {
        this->add("400", "/public/400");
        this->add("401", "/public/401");
        this->add("402", "/public/402");
        this->add("403", "/public/403");
        this->add("404", "/public/404");
        this->add("405", "/public/405");
        this->add("501", "/public/501");
        this->add("502", "/public/502");
        if (serverConfig->find("error_page") == serverConfig->end())
            return;
        std::map<int, std::string> errorPages = *serverConfig->at("error_page").data;
        for (std::map<int, std::string>::const_iterator errorPage = errorPages.begin(); errorPage != errorPages.end(); ++errorPage) {
            std::stringstream ss;
            ss << errorPage->first;
            std::string errorCode;
            ss >> errorCode;
            (*this)[errorCode] = errorPage->second;
        }
    }
};

inline std::map<std::string, Model> createModels(const std::vector<ServerConfig> &configs) {
    std::map<std::string, Model> models;

    for (std::vector<ServerConfig>::const_iterator serverConfig = configs.begin(); serverConfig != configs.end(); ++serverConfig) {
        HashMap locations = *serverConfig->at("location").data;

        for (HashMap::const_iterator locationIter = locations.begin(); locationIter != locations.end(); ++locationIter) {
            std::string fullURL = getFullURL(serverConfig, locationIter->first);
            Model &model = models[fullURL];
            model.add("root", locationIter, serverConfig);
            model.add("index", locationIter, serverConfig);
            model.add("autoindex", locationIter, serverConfig);
            model.add("client_max_body_size", locationIter, serverConfig);
            model.add("fastcgi_pass", locationIter, serverConfig);
            model.addErrorPages(serverConfig);
        }
    }
    return models;
};