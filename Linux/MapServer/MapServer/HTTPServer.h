#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "../Helpers/Assert.h"
#include "../Helpers/String/String.h"
#include "../Helpers/FileSystem/FileSystem.h"

#include "Servlets/Servlet.h"

#include "mongoose/mongoose.h"

namespace MapServer {

class HTTPServer {

public:
	HTTPServer(const bool asynchronousServletExecution = true) :
	    rootDirectory("."),
	    mgx(0),
	    runningServlet(-1),
	    asynchronousServletExecution(asynchronousServletExecution) {
		server = this;
		RegisterMimeType("txt", "text/plain");
		RegisterMimeType("html", "text/html");
		RegisterMimeType("png", "image/png");
		RegisterMimeType("jpg", "image/jpeg");
		RegisterMimeType("js", "application/x-javascript");
		RegisterMimeType("css", "text/css");
	}

	inline void SetRootDirectory(const std::string d) {
		rootDirectory = d;
		if (rootDirectory.empty()) rootDirectory = ".";
		if (rootDirectory[rootDirectory.length()-1] == '/') rootDirectory = rootDirectory.substr(0, rootDirectory.length()-1);
	}

	void Start(const int port, const bool verbose = true) {
	    std::string portString = String::lexicalCast<std::string>(port);
		const char *options[] = {"listening_ports", portString.c_str(), nullptr};
		if (verbose) std::cout << "Starting HTTP Server on port " << portString << ". Press any key to stop." << std::endl;
		mgx = mg_start(&Callback, nullptr, options);
	}

	bool Stop(const bool verbose = true) {
	    if (runningServlet != -1) {
	        kill(runningServlet, SIGKILL);
	        return false;
	    }
		if (verbose) std::cout << "Stopping HTTP Server... " << std::flush;
		mg_stop(mgx);
		if (verbose) std::cout << "OK." << std::endl;
		return true;
	}

	void RegisterMimeType(const std::string extension, const std::string type) {
		mimeTypes[extension] = type;
	}

	void RegisterServlet(Servlet* servlet) {
		servlets.push_back(servlet);
	}

	void Process(mg_connection *conn, const mg_request_info *requestInfo) {
	    std::cout << "\n\033[1;34m[" << requestInfo->remote_ip << "] Requesting " << requestInfo->uri << "\033[0m" << std::endl;
		std::string uri(requestInfo->uri + 1);

		for (int i = 0; i < servlets.size(); ++i) {
			if (servlets[i]->Handler() == uri) {
			    if (asynchronousServletExecution) {
	                waitpid(runningServlet, 0, 0);
	                runningServlet = fork();
	                if (runningServlet == -1) {
	                    error("Failure during fork()!");
	                    exit(1);
	                }
	                if (runningServlet == 0) {
	                    servlets[i]->Process(conn, requestInfo);
	                    exit(0);
	                }
	                waitpid(runningServlet, 0, 0);
	                runningServlet = -1;
	                return;
			    } else {
	                servlets[i]->Process(conn, requestInfo);
	                return;
			    }
			}
		}

		string filename = rootDirectory + "/" + (uri == "" ? IndexFilename : uri);
        std::cout << "No servlet found, trying to serve " << filename << " as a file..." << std::endl;
		if (!FileSystem::isFile(filename)) {
		    filename = "/" + uri;
	        std::cout << "File not found, trying to serve " << filename << " as a file..." << std::endl;
		}
        if (!FileSystem::isFile(filename)) {
            std::cout << "File not found!" << std::endl;
            ServeError(conn, 404, "Not Found", "The file " + filename + " could not be found.");
            return;
        }

		std::ifstream f(filename, std::ios::binary);
		AssertMsg(f.is_open(), "The file " << filename << " could not be opened!");

		string extension(GetFileExtension(filename));
		if (mimeTypes.find(extension) == mimeTypes.end()) extension = "txt";

		std::cout << "Serving with mime-type " << mimeTypes[extension] << "..." << std::endl;
		mg_printf(conn, "HTTP/1.1 200 OK\r\n");
		mg_printf(conn, "Content-Type: %s\r\n\r\n", mimeTypes[extension].c_str());
		while (!f.eof()) {
			char buffer[1024];
			f.read(buffer, sizeof(buffer));
			mg_write(conn, buffer, f.gcount());
		}
	}

protected:
	static void* Callback(mg_event event, mg_connection *conn, const mg_request_info *requestInfo) {
		Assert(server != nullptr);
		if (event == MG_NEW_REQUEST) {
			server->Process(conn, requestInfo);
		}
		return (void*)"";
	}

	static void ServeError(mg_connection *conn, const int errorCode, const string errorString, const string errorMessage) {
		mg_printf(conn, "HTTP/1.1 %d %s\r\nContent-Type: text/plain\r\n\r\n%s", errorCode, errorString.c_str(), errorMessage.c_str());
	}

	string GetFileExtension(const string filename) const {
	    if(filename.find_last_of(".") != string::npos) return filename.substr(filename.find_last_of(".")+1);
	    return "";
	}

protected:
	static HTTPServer *server;
	static const string IndexFilename;
	string rootDirectory;

	mg_context *mgx;

	std::vector<Servlet*> servlets;

	std::map<string, string> mimeTypes;

	pid_t runningServlet;

	const bool asynchronousServletExecution;

};

HTTPServer* HTTPServer::server = NULL;
const string HTTPServer::IndexFilename = "index.html";

}
