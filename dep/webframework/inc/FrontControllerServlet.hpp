#pragma once

#include <map>
#include <string>
#include "ServerConfig.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "Handler.hpp"
#include "Model.hpp"

class HttpServlet {
public:
    HttpServlet() {};
    virtual ~HttpServlet() {};

    virtual void service(const ServerConfig &config, const ServletRequest &request, ServletResponse &response) = 0;
};

class FrontControllerServlet: public HttpServlet {
private:
    std::map<std::string, std::map<std::string, const Handler *> > handlerMappingMap;
    std::map<std::string, HashMap> config;
    std::map<std::string, Model> models;

public:
    FrontControllerServlet(const std::vector<ServerConfig> &configs);
    virtual ~FrontControllerServlet();

    virtual void service(const ServerConfig &config, const ServletRequest &request, ServletResponse &response);

    std::vector<std::string> getAllowedMethods(const HashMap &locationConfig);

    std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator findHandler(const std::string &requestURI);

    Model findModel(const std::string &requestURI);
};
