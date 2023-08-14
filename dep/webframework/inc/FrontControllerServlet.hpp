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
    std::map<std::string, std::map<std::string, const Handler *> > handlerMappingMap;
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
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("GET", new GetHandler(locationConfig)));
                    } else if (*allowMethod == "POST") {
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("POST", new PostHandler(locationConfig)));
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
        for (std::map<std::string, std::map<std::string, const Handler *> >::iterator iter = handlerMappingMap.begin(); iter != handlerMappingMap.end(); ++iter) {
            for (std::map<std::string, const Handler *>::iterator subIter = iter->second.begin();
                subIter != iter->second.end();
                ++subIter
            ) {
                if (subIter->second) {
                    delete subIter->second;
                    subIter->second = 0;
                }
            }
        }
    };

    virtual void service(const ServletRequest &request, ServletResponse &response) {
        std::string requestURI = request.getRequestURI();
        std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator handlerCandidates = this->findHandler(requestURI);
        if (handlerCandidates == this->handlerMappingMap.rend()) {
            response.setStatus(ServletResponse::SC_NOT_FOUND);
            return;
        }
        const Handler *handler = handlerCandidates->second[request.method];
        if (handler == NULL) {
            response.setStatus(ServletResponse::METHOD_NOT_ALLOWED);
            return;
        }
                                                       
        const std::string &requestRoot = handlerCandidates->first;
        std::map<std::string, std::string> paramMap = request.createParamMap(requestRoot);
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
            static std::string arr[] = {"GET"}; // {"GET", "POST", "PUT", "HEAD", "DELETE"};
            int n = sizeof(arr) / sizeof(arr[0]);
            static const std::vector<std::string> ALL_ALLOWED_METHODS(arr, arr + n);
            return ALL_ALLOWED_METHODS;
        }
    }

    std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator findHandler(const std::string &requestURI) {
        for (std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator handlerIter = this->handlerMappingMap.rbegin();
            handlerIter != this->handlerMappingMap.rend();
            ++handlerIter
        ) {
            if (requestURI.find(handlerIter->first) != std::string::npos) {
                return handlerIter;
            }
        }

        return this->handlerMappingMap.rend();
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
