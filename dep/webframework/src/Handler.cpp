#include "Handler.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <ctime>

std::map<std::string, HashMap>::const_iterator Handler::findLocationBlock(const std::string &url, const std::map<std::string, HashMap> &locationBlocks) const {
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

std::vector<std::string> createEnvs(const std::map<std::string, std::string> &paramMap, const Model &model) {
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

	if (paramMap.find("secretHeader") != paramMap.end()) {
		envs.push_back("HTTP_X_SECRET_HEADER_FOR_TEST=" + paramMap.at("secretHeader"));
	} 

	envs.push_back("");
	return envs;
}

std::string execCgi(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) {
	int saveStdin = dup(STDIN_FILENO);
	int saveStdout = dup(STDOUT_FILENO);

	FILE *files[2];
	int fileFds[2];
	files[0] = tmpfile();
	files[1] = tmpfile();
	fileFds[0] = fileno(files[0]);
	fileFds[1] = fileno(files[1]);

	const std::string &requestBody = paramMap.at("body");
	write(fileFds[0], requestBody.c_str(), requestBody.length());
	lseek(fileFds[0], 0, SEEK_SET);

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

        std::vector<char *> argArray;
        argArray.push_back(const_cast<char *>("/usr/bin/python"));
        argArray.push_back(const_cast<char *>(model.at("cgiPath").c_str()));
        argArray.push_back(NULL);

        int res;
        if (model.find("cgiExtension") != model.end() && model.at("cgiExtension") == ".py") {
            res = execve("/usr/bin/python", argArray.data(), envArray.data());
        } else {
		    res = execve(model.at("cgiPath").c_str(), null, envArray.data());

        }
		exit(1);
	}
	else
	{
		const int BUF_SIZE = 100000;
		char	buffer[BUF_SIZE] = {0};

        clock_t delay = 5 * CLOCKS_PER_SEC;
        clock_t start_time = clock();
        int procFlag = -1;
        while (clock() - start_time < delay) {
		    if ((procFlag = waitpid(pid, NULL, WNOHANG)))
                break;
        }
        if (procFlag < 0) {
            response.setStatus(501);
            return model["501"];
        }
		lseek(fileFds[1], 0, SEEK_SET);
		int ret = 1;
		while (ret > 0) {
			memset(buffer, 0, BUF_SIZE);
			ret = read(fileFds[1], buffer, BUF_SIZE - 1);
			response.body.append(std::string(buffer));
		}
	}

	dup2(saveStdin, STDIN_FILENO);
	dup2(saveStdout, STDOUT_FILENO);
	close(saveStdin);
	close(saveStdout);
	fclose(files[0]);
	fclose(files[1]);

	std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");

	size_t start = 0;
	while((start = filepath.find("//", start)) != std::string::npos) {
		filepath.replace(start, 2, "/");
		start += 1;
	}

    size_t headerEnd = response.body.find("\r\n\r\n");
    if (headerEnd == std::string::npos) {
        response.setStatus(501);
        return model["501"];
    }

    response.setStatus(200);
    if (paramMap.at("method") == "POST" || paramMap.at("method") == "PUT") {
        std::ofstream ofs(filepath.c_str());
        ofs << response.body;
    }
	return "cgi";
}

std::string GetHandler::process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
    std::string fullUrl = paramMap.at("fullURL");
    size_t dotPos = fullUrl.find_last_of(".");
    size_t colonPos = fullUrl.find_last_of(":");
    if (dotPos != std::string::npos && colonPos != std::string::npos && colonPos < dotPos) {
        std::string extension = fullUrl.substr(dotPos, fullUrl.length() - dotPos);
        if (model.find("cgiExtension") != model.end() && model.at("cgiExtension") == extension) {
            return execCgi(paramMap, model, response);
        }
    }

    std::string rootPath = "." + model["root"];
    std::string filepath = rootPath + paramMap.at("restOfRequest");
    
    size_t start = 0;
    while((start = filepath.find("//", start)) != std::string::npos) {
        filepath.replace(start, 2, "/");
        start += 1;
    }
    std::cout << "filePath = " << filepath << std::endl;

    std::ifstream ifs(filepath.c_str());

    

    DIR *dir;
    if ((dir = opendir(filepath.c_str()))) {
        closedir(dir);
        if (model.find("autoindex") != model.end()) {
            if (model["autoindex"] == "on") {
                response.setStatus(200);
                return "./public/resources/autoIndex.html";
            }
        }
        std::string innerIndexFileName = filepath + "/" + model["index"];
        size_t start = 0;
        while((start = innerIndexFileName.find("//", start)) != std::string::npos) {
            innerIndexFileName.replace(start, 2, "/");
            start += 1;
        }
        std::ifstream innerIndex(innerIndexFileName.c_str());
        if (innerIndex.good()) {
            response.setStatus(200);
            return innerIndexFileName;
        }
    }else if (ifs.is_open()) {
        response.setStatus(200);
        return filepath;
    }
    response.setStatus(404);
    return model["404"];
};

std::string PostHandler::process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
    std::string fullUrl = paramMap.at("fullURL");
    size_t dotPos = fullUrl.find_last_of(".");
    size_t colonPos = fullUrl.find_last_of(":");
    if (dotPos != std::string::npos && colonPos != std::string::npos && colonPos < dotPos) {
        std::string extension = fullUrl.substr(dotPos, fullUrl.length() - dotPos);
        if (model.find("cgiExtension") != model.end() && model.at("cgiExtension") == extension) {
            return execCgi(paramMap, model, response);
        }
    }

    // if (paramMap.find("Content-Length") != paramMap.end() && paramMap.at("Content-Length") == "0") {
    // 	response.setStatus(400);
    // 	return model["400"];
    // }
    
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

    std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");

    size_t start = 0;
    while((start = filepath.find("//", start)) != std::string::npos) {
        filepath.replace(start, 2, "/");
        start += 1;
    }

    std::ifstream file(filepath.c_str());
    if (file.good()) {
        std::ofstream ofs(filepath.c_str());
        ofs << paramMap.at("body");
        response.setBody(paramMap.at("body"));
        response.addHeader("Location", "/index.html");
        response.setStatus(303);
        return model["303"];
    } else {
        std::ofstream ofs(filepath.c_str());
        ofs << paramMap.at("body");
        response.setBody(paramMap.at("body"));
        response.setStatus(200);
        return model["200"];
    }
};

std::string PutHandler::process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
    std::string filepath = "." + model["root"] + paramMap.at("restOfRequest");

    size_t start = 0;
    while((start = filepath.find("//", start)) != std::string::npos) {
        filepath.replace(start, 2, "/");
        start += 1;
    }

    std::ofstream ofs(filepath.c_str());
    ofs << paramMap.at("body");
    response.setBody(paramMap.at("body"));
    response.setStatus(200);
    return model["200"];
};

std::string DeleteHandler::process(const std::map<std::string, std::string> &paramMap, Model &model, ServletResponse &response) const {
    std::string rootPath = "." + model["root"];
    std::string filepath = rootPath + paramMap.at("restOfRequest");
    
    size_t start = 0;
    while((start = filepath.find("//", start)) != std::string::npos) {
        filepath.replace(start, 2, "/");
        start += 1;
    }
    std::cout << "filePath = " << filepath << std::endl;
    std::ifstream ifs(filepath.c_str());

    if (ifs.is_open()) {
        remove(filepath.c_str());
        response.setStatus(200);
        return model["200"];
    }
    response.setStatus(404);
    return model["404"];
};