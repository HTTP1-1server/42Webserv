#pragma once

#include <string>
#include <sstream>
#include "Model.hpp"

class View {
protected:
    std::string viewName;
public:
    View(const std::string &viewName): viewName(viewName) {};
    virtual ~View() {};

    virtual void render(const Model &model, const ServletRequest &request, ServletResponse &response) const = 0;

    static void errorRender(const std::map<int, std::string> &errors, ServletResponse &response) {
        std::string viewName = errors.at(response.statusCode);
        std::ifstream file(viewName.c_str());
        if (file.good()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            response.setBody(buffer.str());
        }
    }
};

class NormalView: public View {
public:
    NormalView(const std::string &viewName): View(viewName) {};
    virtual ~NormalView() {};
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
        (void)request;
        std::ifstream file(this->viewName.c_str());
        if (file.good()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            response.setBody(buffer.str());
        } else {
            std::ifstream errorfile(model.at("404").c_str());
            std::stringstream buffer;
            buffer << errorfile.rdbuf();
            response.setStatus(404);
            response.setBody(buffer.str());
        }
    };
};

class AutoIndexView: public View {
public:
    AutoIndexView(const std::string &viewName): View(viewName){};
    virtual ~AutoIndexView() {};
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
        std::ifstream autoIndexFile(this->viewName.c_str());
        std::string directory = "." + model.at("root");
        if (autoIndexFile.good()) {
            // std::stringstream buffer;
            // buffer << autoIndexFile.rdbuf();

            // std::string content = buffer.str();
            // int pos = content.find("{FILELIST}");
            // content.replace(pos, 10, this->getDirContents(directory));
            response.setBody("0");
        }
    };

    std::string getDirContents(const std::string &dir_name) const {
        std::string content;
        struct dirent *ent;

        DIR *dir = opendir(dir_name.c_str());
        if (dir == NULL) {
            return "";
        }
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName(ent->d_name);
            content.append("<ul>" + fileName + "</ul>");
        }
        closedir(dir);
        return content;
    }
};

class CgiView: public View {
public:
    CgiView(const std::string &viewName): View(viewName){};
    virtual ~CgiView() {};
    // Status: 200 OK
    // Status: 404 Not FOund
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
        (void)model;
        (void)request;
        size_t headerEnd = response.body.find("\r\n\r\n");
		std::stringstream headerStream(response.body.substr(0, headerEnd));
		Parser httpParser;
		std::map<std::string, std::string> headers = httpParser.parseHttpHeader(headerStream);
		response.header = headers;
        response.body = response.body.substr(headerEnd + 4, response.body.length() - 4 - headerEnd);
    };
};

View *viewResolver(const std::string &viewName) {
    if (viewName.find("autoIndex.html") != std::string::npos) {
        return new AutoIndexView(viewName);
    } else if (viewName == "cgi") {
        return new CgiView(viewName);
    }
    return new NormalView(viewName);
}