#pragma once

#include <map>
#include "ConfigData.hpp"

class ServerConfigData {
private:
    std::map<std::string, ConfigData> configs;
public:
    ServerConfigData(/* args */);
    ~ServerConfigData();
};

ServerConfigData::ServerConfigData(/* args */) {
}

ServerConfigData::~ServerConfigData() {
}
