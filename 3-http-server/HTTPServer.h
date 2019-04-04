#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cstring>
#include <string>

class HTTPServer {
public:
	HTTPServer();
	~HTTPServer();

	void makeNonBlocking();
	void listenOnAddress(int port, const char* ip);
	void serve();

private:
	void handleEvents();
	void startNewConnection();
	bool receiveMessage();
	void reactToMessage();
	void checkMethod();
	bool endOfRequest() { return (isGet && query.find("\r\n\r\n") != std::string::npos) || (!isGet && contentLeft == 0); }
	bool endOfPostHeader() { return !isGet && query.find("\r\n\r\n") != std::string::npos; }
	void setContentInfo();
	void sendResponse();
	std::string getFilePath();
	std::string formAnswer(std::string filePath);
	std::vector<char> readFile (const char* path);
	void printInfo();
	void closeConnection();

	sockaddr_in sin;
	unsigned size;
	const int sockfd;
	std::vector<pollfd> fds;

	std::string buffer;
  std::string query = "";
  bool isGet;
  int contentLength = 0;
  int contentLeft = 100;
	int currentFdIndex = 0;
};

#endif /*HTTPSERVER_H*/