#pragma once

#include <map>
#include <string>
#include "ServerConfig.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "utils/Handler.hpp"
#include "View.hpp"

class GenericServlet {
private:
public:
    GenericServlet() {};
    virtual ~GenericServlet() {};

    virtual void service(const ServletRequest &request, ServletResponse &response) = 0;
};

class HttpServlet: public GenericServlet {
public:
    HttpServlet() {};
    virtual ~HttpServlet() {};

    virtual void service(const ServletRequest &request, ServletResponse &response) = 0;
};

class FrontControllerServlet: public HttpServlet {
private:
    std::map<std::pair<std::string, std::string>, const Handler *> handlerMappingMap;
    // const std::vector<ServerConfig> &config;
    std::map<std::string, HashMap> config;

public:
    FrontControllerServlet(const std::vector<ServerConfig> &configs) {
        for (std::vector<ServerConfig>::const_iterator serverConfig = configs.begin(); serverConfig != configs.end(); ++serverConfig) {
            HashMap locations = *serverConfig->at("location").data;

            for (HashMap::const_iterator locationIter = locations.begin(); locationIter != locations.end(); ++locationIter) {
                HashMap locationConfig = *locationIter->second.data;
                HashMap locationBlock = locationConfig;
                if (locationBlock.find("root") == locationBlock.end()) {
                    std::string rootPath = *serverConfig->at("root").data;
                    locationBlock.insert(std::make_pair("root", UniquePtr<Any>(AnyType<std::string>(rootPath))));
                }
                if (locationBlock.find("index") == locationBlock.end()) {
                    std::string indexPath = *serverConfig->at("index").data;
                    locationBlock.insert(std::make_pair("index", UniquePtr<Any>(AnyType<std::string>(indexPath))));
                }

                std::map<int, std::string> errorPages;
                errorPages.insert(std::make_pair(400, "/public/400"));
                errorPages.insert(std::make_pair(401, "/public/401"));
                errorPages.insert(std::make_pair(402, "/public/402"));
                errorPages.insert(std::make_pair(403, "/public/403"));
                errorPages.insert(std::make_pair(404, "/public/404"));
                errorPages.insert(std::make_pair(405, "/public/405"));
                errorPages.insert(std::make_pair(501, "/public/501"));
                errorPages.insert(std::make_pair(502, "/public/502"));

                for (std::map<int, std::string>::iterator errorIter = errorPages.begin(); errorIter != errorPages.end(); ++errorIter) {
                    if (serverConfig->find("error_page") == serverConfig->end()) {
                        break;
                    }
                    std::map<int, std::string> serverErrorPages = *serverConfig->at("error_page").data;
                    if (serverErrorPages.find(errorIter->first) != serverErrorPages.end()) {
                        errorIter->second = serverErrorPages.at(errorIter->first);
                    }
                }
                locationBlock.insert(std::make_pair("error_page", UniquePtr<Any>(AnyType<std::map<int, std::string> >(errorPages))));
                
                std::string fullURL = this->getFullURL(serverConfig, locationIter->first);
                this->config.insert(std::make_pair(fullURL, locationBlock));

                std::vector<std::string> allowMethods = this->getAllowedMethods(locationConfig);
                for (std::vector<std::string>::const_iterator allowMethod = allowMethods.begin(); allowMethod != allowMethods.end(); ++allowMethod) {
                    if (*allowMethod == "GET") {
                        this->handlerMappingMap.insert(std::make_pair(std::make_pair(fullURL, "GET"), new GetHandler(locationConfig)));
                    } else if (*allowMethod == "POST") {
                        this->handlerMappingMap.insert(std::make_pair(std::make_pair(fullURL, "POST"), new PostHandler(locationConfig)));
                    }
                    //  else if (*allowMethod == "PUT") {
                    //     this->handlerMappingMap.insert(std::make_pair(std::make_pair(fullURL, "PUT"), new PutHandler(locationConfig)));
                    // } else if (*allowMethod == "HEAD") {
                    //     this->handlerMappingMap.insert(std::make_pair(std::make_pair(fullURL, "HEAD"), new HeadHandler(locationConfig)));
                    // } else if (*allowMethod == "DELETE") {
                    //     this->handlerMappingMap.insert(std::make_pair(std::make_pair(fullURL, "DELETE"), new DeleteHandler(locationConfig)));
                    // }
                }
            }
        }

    };
    virtual ~FrontControllerServlet() {
        for (std::map<std::pair<std::string, std::string>, const Handler *>::iterator iter = handlerMappingMap.begin(); iter != handlerMappingMap.end(); ++iter) {
            if (iter->second) {
                delete iter->second;
                iter->second = 0;
            }
        }
    };

    virtual void service(const ServletRequest &request, ServletResponse &response) {
        std::string requestURI = request.getRequestURI();
        const Handler *handler = this->findHandler(requestURI, request.method);
        if (handler == NULL) {
            response.setStatus(ServletResponse::SC_NOT_FOUND);
            return;
        }

        std::map<std::string, std::string> paramMap = request.createParamMap();
        std::string viewName = handler->process(paramMap, this->config);

        std::cout << "VIEWNAME: " << viewName << std::endl;

        // View view = View::viewResolver(viewName);
        // view.render(request, response);

        // ServletResponse responseMessage(response);
        // socketManager->sendResponseMessage(connectSd, responseMessage);
    };

    std::string getFullURL(std::vector<ServerConfig>::const_iterator serverConfig, const std::string &locationDir) {
        std::string host = *serverConfig->at("host").data;
        int port = *serverConfig->at("listen").data;
        std::stringstream ssPort;
        ssPort << port;
        std::string portString;
        ssPort >> portString;
        return host + ":" + portString + locationDir;
    }

    std::vector<std::string> getAllowedMethods(const HashMap &locationConfig) {
        if (locationConfig.find("allowed_methods") != locationConfig.end()) {
            std::vector<std::string> allowedMethod = *locationConfig.at("allowed_methods").data;
            return allowedMethod;
        } else {
            static std::string arr[] = {"GET", "POST", "PUT", "HEAD", "DELETE"};
            int n = sizeof(arr) / sizeof(arr[0]);
            static const std::vector<std::string> ALL_ALLOWED_METHODS(arr, arr + n);
            return ALL_ALLOWED_METHODS;
        }
    }

    const Handler *findHandler(const std::string &requestURI, const std::string &method) {
        const Handler *handler = NULL;
        std::string handlerName;

        for (std::map<std::pair<std::string, std::string>, const Handler *>::const_iterator handlerIter = this->handlerMappingMap.begin();
            handlerIter != this->handlerMappingMap.end();
            ++handlerIter
        ) {
            if (requestURI.find(handlerIter->first.first) != std::string::npos && method == handlerIter->first.second) {
                if (handlerIter->first.first.length() >= handlerName.length()) {
                    handlerName = handlerIter->first.first;
                    handler = handlerIter->second;
                }
            }
        }
        return handler;
    }
};
