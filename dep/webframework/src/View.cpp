#include "View.hpp"
#include <sys/types.h>
#include <dirent.h>

void View::errorRender(const std::map<int, std::string> &errors, ServletResponse &response) {
    std::string viewName = errors.at(response.statusCode);
    std::ifstream file(viewName.c_str());
    if (file.good()) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        response.setBody(buffer.str());
    }
};

void NormalView::render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
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

void AutoIndexView::render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
    (void)request;
    std::ifstream autoIndexFile(this->viewName.c_str());
    std::string directory = "." + model.at("root");
    if (autoIndexFile.good()) {
        std::stringstream buffer;
        buffer << autoIndexFile.rdbuf();

        std::string content = buffer.str();
        int pos = content.find("{FILELIST}");
        content.replace(pos, 10, this->getDirContents(directory));
        response.setBody(content);
    }
};

std::string AutoIndexView::getDirContents(const std::string &dir_name) const {
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

void CgiView::render(const Model &model, const ServletRequest &request, ServletResponse &response) const {
    (void)model;
    (void)request;
    size_t headerEnd = response.body.find("\r\n\r\n");
    std::cout << "HE: " << headerEnd << std::endl;
    if (headerEnd == std::string::npos) {
        headerEnd = 0;
    }
    std::stringstream headerStream(response.body.substr(0, headerEnd));
    Parser httpParser;
    std::map<std::string, std::string> headers = httpParser.parseHttpHeader(headerStream);
    response.header = headers;
    response.body = response.body.substr(headerEnd + 4, response.body.length() - 4 - headerEnd);
};

View *viewResolver(const std::string &viewName) {
    if (viewName.find("autoIndex.html") != std::string::npos) {
        return new AutoIndexView(viewName);
    } else if (viewName == "cgi") {
        return new CgiView(viewName);
    }
    return new NormalView(viewName);
}