#pragma once

#include <string>
#include "Model.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"

class View {
protected:
    std::string viewName;
public:
    View(const std::string &viewName): viewName(viewName) {};
    virtual ~View() {};

    virtual void render(const Model &model, const ServletRequest &request, ServletResponse &response) const = 0;

    static void errorRender(const std::map<int, std::string> &errors, ServletResponse &response);
};

class NormalView: public View {
public:
    NormalView(const std::string &viewName): View(viewName) {};
    virtual ~NormalView() {};
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const;
};

class AutoIndexView: public View {
public:
    AutoIndexView(const std::string &viewName): View(viewName){};
    virtual ~AutoIndexView() {};
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const;

    std::string getDirContents(const std::string &dir_name) const;
};

class CgiView: public View {
public:
    CgiView(const std::string &viewName): View(viewName){};
    virtual ~CgiView() {};
    // Status: 200 OK
    // Status: 404 Not FOund
    void render(const Model &model, const ServletRequest &request, ServletResponse &response) const;
};

View *viewResolver(const std::string &viewName);