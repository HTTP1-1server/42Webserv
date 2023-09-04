#include "FrontControllerServlet.hpp"
#include "View.hpp"
#include <set>

FrontControllerServlet::FrontControllerServlet(const std::vector<ServerConfig> &configs) {
    this->models = createModels(configs);

    for (std::vector<ServerConfig>::const_iterator serverConfig = configs.begin(); serverConfig != configs.end(); ++serverConfig) {
        HashMap locations = *serverConfig->at("location").data;

        for (HashMap::const_iterator locationIter = locations.begin(); locationIter != locations.end(); ++locationIter) {
            HashMap locationConfig = *locationIter->second.data;
            std::set<std::string> hosts;
            std::string host = *serverConfig->at("host").data;
            hosts.insert("localhost");
            hosts.insert("127.0.0.1");
            hosts.insert("[::1]");
            hosts.insert(host);
            for (std::set<std::string>::iterator hostIter = hosts.begin(); hostIter != hosts.end(); ++hostIter) {
                std::string fullURL = getFullURL(serverConfig, locationIter->first);
                fullURL.replace(0, host.length(), *hostIter);
                std::cout << "FULLURL: " << fullURL << std::endl;
                std::vector<std::string> allowMethods = this->getAllowedMethods(locationConfig);

                for (std::vector<std::string>::const_iterator allowMethod = allowMethods.begin(); allowMethod != allowMethods.end(); ++allowMethod) {
                    if (*allowMethod == "GET") {
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("GET", new GetHandler(locationConfig)));
                    } else if (*allowMethod == "POST") {
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("POST", new PostHandler(locationConfig)));
                    } else if (*allowMethod == "PUT") {
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("PUT", new PutHandler(locationConfig)));
                    } else if (*allowMethod == "DELETE") {
                        std::map<std::string, const Handler *> &handlers = this->handlerMappingMap[fullURL];
                        handlers.insert(std::make_pair("DELETE", new DeleteHandler(locationConfig)));
                    }
                }
            }
        }
    }

};

FrontControllerServlet::~FrontControllerServlet() {
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

void FrontControllerServlet::service(const ServerConfig &config, const ServletRequest &request, ServletResponse &response) {
    std::string requestURI = request.getRequestURI();

    const Handler *handler = NULL;
    std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator handlerCandidates = this->findHandler(requestURI);
    if (handlerCandidates == this->handlerMappingMap.rend()) {
        response.setStatus(ServletResponse::SC_NOT_FOUND);
    } else {
        handler = handlerCandidates->second[request.method];
        if (handler == NULL) {
            response.setStatus(ServletResponse::METHOD_NOT_ALLOWED);
        }
    }
    if (response.statusCode >= 400) {
        std::map<int, std::string> &errors = *config.at("error_page").data;
        View::errorRender(errors, response);
    } else {
        const std::string &requestRoot = handlerCandidates->first;
        std::map<std::string, std::string> paramMap = request.createParamMap(requestRoot);
        Model model = findModel(requestURI);
        std::string viewName = handler->process(paramMap, model, response);

        std::cout << "viewName" << viewName << std::endl;

        View *view = viewResolver(viewName);
        view->render(model, request, response);
        delete view;
    }
};

std::vector<std::string> FrontControllerServlet::getAllowedMethods(const HashMap &locationConfig) {
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

std::map<std::string, std::map<std::string, const Handler *> >::reverse_iterator FrontControllerServlet::findHandler(const std::string &requestURI) {
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

Model FrontControllerServlet::findModel(const std::string &requestURI) {
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