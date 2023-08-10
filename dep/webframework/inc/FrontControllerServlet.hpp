#pragma once

#include <map>
#include <string>
#include "ServerConfig.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "utils/Handler.hpp"
#include "View.hpp"
#include "Model.hpp"

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
    std::map<std::string, HashMap> config;
    std::map<std::string, Model> models;

public:

    FrontControllerServlet(const std::vector<ServerConfig> &configs) {
        this->models = createModels(configs);

        for (std::vector<ServerConfig>::const_iterator serverConfig = configs.begin(); serverConfig != configs.end(); ++serverConfig) {
            HashMap locations = *serverConfig->at("location").data;

            for (HashMap::const_iterator locationIter = locations.begin(); locationIter != locations.end(); ++locationIter) {
                HashMap locationConfig = *locationIter->second.data;                
                std::string fullURL = getFullURL(serverConfig, locationIter->first);
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
        Model model = findModel(request.getRequestURI());
        std::string viewName = handler->process(paramMap, model, response);

        std::cout << "VIEWNAME: " << viewName << std::endl;

        View *view = viewResolver(viewName);
        view->render(model, request, response);
        delete view;
    };

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

    Model findModel(const std::string &requestURI) {
        const Model *model = NULL;
        std::string locationPath;

        for (std::map<std::string, Model>::const_iterator modelIter = this->models.begin();
            modelIter != this->models.end();
            ++modelIter
        ) {
            if (requestURI.find(modelIter->first) != std::string::npos) {
                if (modelIter->first.length() >= locationPath.length()) {
                    locationPath = modelIter->first;
                    model = &modelIter->second;
                }
            }
        }
        return *model;
    }
};
