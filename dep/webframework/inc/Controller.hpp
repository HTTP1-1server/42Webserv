#pragma once

#include <utility>
#include <vector>
#include <algorithm>
#include "ServletRequest.hpp"
#include "Validator.hpp"


class Controller {
protected:
    const HashMap &directives;
public:
    Controller(const HashMap &directives): directives(directives) {
    };
    ~Controller() {};

    std::pair<ModelView, Response> process(const ServletRequest &request, const Validators &validators) {};
};

class IndexController: Controller {
private:
public:
    IndexController(const HashMap &directives): Controller(directives) {

    };
    ~IndexController() {};

    std::pair<ModelView, Response> process(const ServletRequest &request, const Validators &validators) {
        KeyList directives = this->directives.getKeys();
        for (KeyList::const_iterator directive = directives.begin(); directive != directives.end(); ++directive) {
            const ValidateFn &validate = validators.at(*directive);
            std::pair<ModelView, Response> pair = validate(this->directives, request);
            // if (pair.second.getStatusCode == -1) {
            if (pair.second[0] != '4') {
                continue;
            } else {
                return pair;
            }
        }

        std::string indexHtml = "/public/index.html";
        try {
            std::vector<std::string> allowed_methods = *this->directives.at("allowed_methods").data;
            if(std::find(allowed_methods.begin(), allowed_methods.end(), request.method) == allowed_methods.end()) {
                // return modelview = 404view, response = 404 error page
            }

            std::vector<std::string> confIndex = *this->directives.at("index").data;
            if(std::find(confIndex.begin(), confIndex.end(), request.index) != confIndex.end()) {
                // return modelview = 404view, response = 404 error page
                indexHtml = confIndex;
            }
            // modelview = 인덱스veiw, response = requset.url에 있는 인덱스 페이지
        } catch (const std::exception &e) {
            
        }
    };
    // HashMap<String, <ErrorModelView, ErrorResponse> Fn(request) > validityChecker;
};

std::<Model