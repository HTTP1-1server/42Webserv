#pragma once

#include <map>
#include <string>
#include <utility>
#include <vector>
#include <fstream>
#include <algorithm>
#include "ServletRequestRequest.hpp"

typedef std::string ModelView;
typedef std::string Response;
typedef std::pair<ModelView, Response> (*ValidateFn)(const HashMap &, const ServletRequest &);
typedef std::map<std::string, ValidateFn> Validators;

// TODO: convert to a singlton class
class Validator {
public:
    const Validators &getHashMap() {
        return this->validators;
    }

    Validator() {
        validators.insert(std::make_pair("allowed_methods", &allowedMethodsValidator));
        validators.insert(std::make_pair("fastcgi_pass", &fastcgiPassValidator));
        validators.insert(std::make_pair("client_max_body_size", &clientMaxBodySizeValidator));
        validators.insert(std::make_pair("root", &rootValidator));
        validators.insert(std::make_pair("index", &indexValidator));
    };
    ~Validator() {};

private:
    Validators validators;
};

std::pair<ModelView, Response> allowedMethodsValidator(const HashMap &directives, const ServletRequest &request) {
    std::vector<std::string> allowed_methods = *directives.at("allowed_methods").data;
    if(std::find(allowed_methods.begin(), allowed_methods.end(), request.method) == allowed_methods.end()) {
        return std::make_pair("ErrorModelView", "405");
    }
    return std::make_pair("SuccessModelView", "200");
}

std::pair<ModelView, Response> fastcgiPassValidator(const HashMap &directives, const ServletRequest &request) {
    std::string cgiPath = *directives.at("fastcgi_pass").data;
    if(std::ifstream(cgiPath).fail()) {
        return std::make_pair("ErrorModelView", "404");
    }
    return std::make_pair("SuccessModelView", "200");
}

std::pair<ModelView, Response> clientMaxBodySizeValidator(const HashMap &directives, const ServletRequest &request) {
    int bodyLimit = *directives.at("client_max_body_size").data;
    if(request.body.length() > bodyLimit) {
        return std::make_pair("ErrorModelView", "413");
    }
    return std::make_pair("SuccessModelView", "200");
}

std::pair<ModelView, Response> rootValidator(const HashMap &directives, const ServletRequest &request) {
    std::string rootPath = *directives.at("root").data;
    // TODO: 루트패스가 위험한 영역일 경우 거르기,
    return std::make_pair("SuccessModelView", "200");
}

std::pair<ModelView, Response> indexValidator(const HashMap &directives, const ServletRequest &request) {
    std::string indexPath = *directives.at("index").data;
    std::string rootPath;
    try {
        std::string temp = *directives.at("root").data;
        rootPath = temp;
        if (*rootPath.rbegin() != '/')
            rootPath.push_back('/');
    }
    catch(const std::exception& e) {
        rootPath = std::string("/");
    }
    if(std::ifstream(rootPath + indexPath).fail()) {
        return std::make_pair("ErrorModelView", "404");
    }
    return std::make_pair("SuccessModelView", "200");
}