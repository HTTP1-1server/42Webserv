#pragma once

#include "Types.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <ctime>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstring>
#include <dirent.h>
#include <sys/types.h>
#include "SocketManager.hpp"
#include "ServletRequest.hpp"
#include "ServletResponse.hpp"
#include "Model.hpp"
#include "unistd.h"
#include "sys/wait.h"

class Handler {
protected:
public:
    Handler() {};
    virtual ~Handler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const = 0;

	std::map<std::string, HashMap>::const_iterator findLocationBlock(const std::string &url, const std::map<std::string, HashMap> &locationBlocks) const {
		std::map<std::string, HashMap>::const_iterator location = locationBlocks.begin();

        for (std::map<std::string, HashMap>::const_iterator locationIter = locationBlocks.begin();
            locationIter != locationBlocks.end();
            ++locationIter
        ) {
            if (url.find(locationIter->first) != std::string::npos) {
                if (locationIter->first.length() >= location->first.length()) {
                    location = locationIter;
                }
            }
        }
        return location;
	}
};


inline std::vector<std::string> createEnvs(const std::map<std::string, std::string> &paramMap, const Model &model) {
	std::string requestMethod = paramMap.at("method");
	std::string cgiPath = model.at("cgiPath");
	std::string port = model.at("listen");

	std::vector<std::string> envs;
	envs.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envs.push_back("SERVER_NAME=LeeYongSeong");
	envs.push_back("SERVER_SOFTWARE=Webserv/1.0");
	envs.push_back("REDIRECT_STATUS=200");
	envs.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envs.push_back("SERVER_SOFTWARE=Webserv/1.0");
	envs.push_back("REQUEST_METHOD=" + requestMethod);
	envs.push_back("REQUEST_URI=" + cgiPath);
	envs.push_back("SCRIPT_NAME=" + cgiPath);
	envs.push_back("SCRIPT_FILENAME=" + cgiPath);
	envs.push_back("PATH_INFO=" + cgiPath);
	envs.push_back("PATH_TRANSLATED=" + cgiPath);
	envs.push_back("SERVER_PORT=" + port);
	envs.push_back("");
	return envs;
}

inline std::string execCgi(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) {
	int saveStdin = dup(STDIN_FILENO);
	int saveStdout = dup(STDOUT_FILENO);

	// int pipefd[2][2];
	// pipe(pipefd[0]);
	// pipe(pipefd[1]);

	FILE *files[2];
	int fileFds[2];
	files[0] = tmpfile();
	files[1] = tmpfile();
	fileFds[0] = fileno(files[0]);
	fileFds[1] = fileno(files[1]);

	const std::string &requestBody = paramMap.at("body");
	write(fileFds[0], requestBody.c_str(), requestBody.length());
	lseek(fileFds[0], 0, SEEK_SET);

	std::cout << "CgiHandler:" << model.at("cgiPath") << std::endl;
	pid_t pid = fork();

	if (pid < 0)
	{
		std::cerr << "fork error" << std::endl;
		return model["501"];
	}
	else if (pid == 0)
	{
		dup2(fileFds[0], STDIN_FILENO);
		dup2(fileFds[1], STDOUT_FILENO);
		// dup2(pipefd[0][0], STDIN_FILENO);
		// dup2(pipefd[1][1], STDOUT_FILENO);
		// close(pipefd[0][0]);
		// close(pipefd[0][1]);
		// close(pipefd[1][0]);
		// close(pipefd[1][1]);
		
		std::vector<std::string> envs = createEnvs(paramMap, model);
		std::vector<char *> envArray;
		char * const * null = NULL;
		for (std::vector<std::string>::iterator env = envs.begin(); env != envs.end(); ++env) {
			if (env->empty()) {
				envArray.push_back(NULL);
			} else {
				envArray.push_back(&(*env)[0]);
			}
		}
		std::cerr << "BEFORE EXECV: " << model.at("cgiPath").c_str() << std::endl;
		execve(model.at("cgiPath").c_str(), null, envArray.data());
		std::cerr << "AFTER EXECV: " << model.at("cgiPath").c_str() << std::endl;
		exit(1);
	}
	else
	{
		// close(pipefd[0][0]);
	

		// const std::string &requestBody = paramMap.at("body");
		// write(pipefd[0][1], requestBody.c_str(), requestBody.length());
		// close(pipefd[0][1]);
		// close(pipefd[1][1]);
		const int BUF_SIZE = 100000;
		char	buffer[BUF_SIZE] = {0};

		waitpid(pid, NULL, 0);
		lseek(fileFds[1], 0, SEEK_SET);
		int ret = 1;
		while (ret > 0) {
			memset(buffer, 0, BUF_SIZE);
			ret = read(fileFds[1], buffer, BUF_SIZE - 1);
			std::cerr << "CHILD: " << ret << std::endl;
			response.body.append(std::string(buffer));
		}
	}


	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	std::cout << "CGI RES: " << response.body << std::endl;
	// close(pipefd[1][0]);
	close(saveStdin);
	close(saveStdout);

	std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");
	std::ofstream ofs(filepath.c_str());
	ofs << response.body;
	response.setStatus(200);
	return "cgi";
}

class GetHandler: public virtual Handler {
private:
public:
	const HashMap &config;

    GetHandler(const HashMap &config): config(config) {

	};
    virtual ~GetHandler() {};
    std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {

		std::string fullUrl = paramMap.at("fullURL");
		size_t dotPos = fullUrl.find_last_of(".");
		if (dotPos != std::string::npos) {
			std::string extension = fullUrl.substr(dotPos, fullUrl.length() - dotPos);
			std::cout << "WTF EXT: " << extension << std::endl;
			if (model.find("cgiExtension") != model.end() && model.at("cgiExtension") == extension) {
				std::cout << "WTF2 PATH: " << model.at("cgiPath") << std::endl;
				return execCgi(paramMap, model, response);
			}
		}

		std::string rootPath = "." + model["root"];
		std::string filepath = rootPath + paramMap.at("restOfRequest");
		std::ifstream ifs(filepath.c_str());
		
		// std::cout << "FILEPATH: " << filepath << std::endl;
		DIR *dir;
		if ((dir = opendir(filepath.c_str()))) {
			if (model.find("autoindex") != model.end()) {
				if (model["autoindex"] == "on") {
					response.setStatus(200);
					return "./public/resources/autoIndex.html";
				}
			}
			closedir(dir);
			std::string innerIndexFileName = filepath + "/" + model["index"];
			std::ifstream innerIndex(innerIndexFileName.c_str());
			if (innerIndex.good()) {
				response.setStatus(200);
				return filepath + "/" + model["index"];
			}
		}else if (ifs.is_open()) {
			response.setStatus(200);
			return filepath;
		}
		response.setStatus(404);
		return model["404"];
    };
};

class PostHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PostHandler(const ServerConfig &config): config(config) {

	};
    ~PostHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
		std::string fullUrl = paramMap.at("fullURL");
		size_t dotPos = fullUrl.find_last_of(".");
		if (dotPos != std::string::npos) {
			std::string extension = fullUrl.substr(dotPos, fullUrl.length() - dotPos);
			std::cout << "WTF EXT: " << extension << std::endl;
			if (model.find("cgiExtension") != model.end() && model.at("cgiExtension") == extension) {
				std::cout << "WTF2 PATH: " << model.at("cgiPath") << std::endl;
				return execCgi(paramMap, model, response);
			}
		}

		if (paramMap.find("Content-Length") != paramMap.end() && paramMap.at("Content-Length") == "0") {
			response.setStatus(400);
			return model["400"];
		}
		
		if (model.find("client_max_body_size") != model.end()) {
			std::stringstream ss;
			ss << paramMap.at("bodyLength");
			int requestLength;
			ss >> requestLength;

			std::stringstream ss2;
			ss2 << model.at("client_max_body_size");
			int limit;
			ss2 >> limit;

			if (requestLength > limit) {
				response.setStatus(413);
				return model["413"];
			}
		}

		if (paramMap.at("body").empty()) {
			int statusCode = 400; // 테스터 돌릴때 두번째 테스트 POST empty body 405가 정답
			std::stringstream ss;
			std::string errorCode;
			ss << statusCode;
			ss >> errorCode;
			response.setStatus(statusCode);
			return model[errorCode];
		}

		std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");
		std::ofstream ofs(filepath.c_str());
		ofs << paramMap.at("body");
		response.setBody(paramMap.at("body"));
		response.setStatus(200);
		return model["200"];
    };
};

class PutHandler: public virtual Handler {
private:
public:
	const ServerConfig &config;

    PutHandler(const ServerConfig &config): config(config) {};
    virtual ~PutHandler() {};
    virtual std::string process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
		std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");
		// std::string filepath = "." + paramMap.at("requestRoot") + "/" + paramMap.at("restOfRequest");
		// std::cout << "PUT filename: " << filepath << std::endl;
		std::ofstream ofs(filepath.c_str());
		ofs << paramMap.at("body");
		response.setBody(paramMap.at("body"));
		response.setStatus(200);
		return model["200"];
    };
};

// localhost:8888/put_test/hello.txt

// 1. Reqeust (Chunked)Body -> 파싱 -> 완전한 길이의 body로 만드는 거
// 2. location /put_test {} <- root 디렉티브가 없음 그럼 server의 root("/")를 바라보게되고,
//		reqeust /put_test/create_file -> 파일 생성 위치가 /put_test/create_file 이렇게 나와야하는데,
//		우리 프로그램에서는 /create_file 이렇게 나옴
//		paramMap.at("requestRoot") = localhost:8888/put_test//
