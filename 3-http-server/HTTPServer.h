#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <unistd.h>
#include <poll.h>
#include <vector>
#include <cstring>
#include <string>
#include "TCPSocket.h"

class HTTPServer
{
public:
  HTTPServer(TCPSocket &socket);
  ~HTTPServer();

  void serve();

private:
  void handleEvents();
  void waitForClient();
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
  std::vector<char> readFile(const char *path);
  void printInfo();
  void closeConnection();

  TCPSocket socket;
  std::vector<pollfd> clients;

  std::string buffer;
  std::string query = "";
  bool isGet;
  int contentLength = 0;
  int contentLeft = 100;
  int currentClientIndex = 0;
};

#endif /*HTTPSERVER_H*/