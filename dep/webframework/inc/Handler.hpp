#pragma once

#include <string>
#include "ServletResponse.hpp"
#include "Model.hpp"

class Handler {
protected:
public:
    Handler() {};
    virtual ~Handler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const = 0;

	std::map<std::string, HashMap>::const_iterator findLocationBlock(const std::string &url, const std::map<std::string, HashMap> &locationBlocks) const;
};

std::vector<std::string> createEnvs(const std::map<std::string, std::string> &paramMap, const Model &model);

std::string execCgi(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response);

class GetHandler: public virtual Handler {
private:
public:
	const HashMap &config;

    GetHandler(const HashMap &config): config(config) {};
    virtual ~GetHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const;
};

class PostHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PostHandler(const ServerConfig &config): config(config) {};
    virtual ~PostHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const;
};

class PutHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PutHandler(const ServerConfig &config): config(config) {};
    virtual ~PutHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const;
};