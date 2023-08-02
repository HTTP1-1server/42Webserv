#pragma once

#include <string>

class View {
private:
    
public:
    View(/* args */);
    ~View();

    static View viewResolver(const std::string &viewName) {
        return View();
    }
};
