#include "HTTPServer.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdio.h>

HTTPServer::HTTPServer(TCPSocket &socket)
{
  this->socket = socket;
}

HTTPServer::~HTTPServer()
{
  for (auto &fd : clients)
    close(fd.fd);
}

void HTTPServer::serve()
{
  clients.push_back({socket, POLLIN, 0});

  while (true)
    handleEvents();
}

void HTTPServer::handleEvents()
{
  waitForClient();

  for (unsigned i = 0; i < clients.size(); ++i)
    if (clients[i].revents & POLLIN)
    {
      if (clients[i].fd == socket)
        startNewConnection();
      else
      {
        currentClientIndex = i;
        if (receiveMessage())
          reactToMessage();
      }
    }
}

void HTTPServer::waitForClient()
{
  if (poll(clients.data(), clients.size(), -1) == -1)
  {
    perror("poll");
    return;
  }
}

void HTTPServer::startNewConnection()
{
  sockaddr_in client;
  memset(&client, 0, sizeof(client));
  unsigned int client_len = sizeof(client);

  int receiver = accept(socket, (sockaddr *)&client, &client_len);
  if (receiver == -1)
    perror("accept");
  else
    clients.push_back({receiver, POLLIN | POLLOUT, 0});
}

bool HTTPServer::receiveMessage()
{
  buffer.resize(100);

  int received = recv(clients[currentClientIndex].fd, const_cast<char *>(buffer.data()), buffer.length(), 0);
  if (received == -1)
  {
    perror("recv");
    return false;
  }

  if (received == 0)
  {
    closeConnection();
    return false;
  }

  buffer.resize(received);
  return true;
}

void HTTPServer::reactToMessage()
{
  checkMethod();
  query += buffer;

  if (endOfPostHeader())
  {
    setContentInfo();
  }

  if (endOfRequest())
  {
    printInfo();
    sendResponse();
    closeConnection();
    query = "";
  }
}

void HTTPServer::checkMethod()
{
  if (query.empty())
    isGet = (buffer.find("GET") == 0);
}

void HTTPServer::setContentInfo()
{
  if (contentLength == 0)
  {
    std::string lengthStr = query.substr(query.find("Content-Length") + 16);
    lengthStr = lengthStr.substr(0, lengthStr.find("\r\n"));
    contentLength = std::atoi(lengthStr.c_str());
  }
  int contentReceived = query.length() - query.find("\r\n\r\n") - 4;
  contentLeft = contentLength - contentReceived;
}

void HTTPServer::printInfo()
{
  std::cout << query.substr(0, query.find("\r\n\r\n")) << std::endl;
  if (!isGet)
  {
    std::string content = query.substr(query.find("\r\n\r\n") + 4);
    std::cout << "// received: " << content << std::endl;
  }
}

void HTTPServer::sendResponse()
{
  std::string filePath = getFilePath();
  std::string answer = formAnswer(filePath);
  if (send(clients[currentClientIndex].fd, answer.data(), answer.length(), MSG_NOSIGNAL) == -1)
    perror("send");
  std::cout << "// sent: " << filePath << std::endl << std::endl;
}

std::string HTTPServer::getFilePath()
{
  std::string filePath = "Strona";
  int begin = isGet ? 4 : 5;
  int length = query.find("HTTP/") - (isGet ? 5 : 6);
  filePath += query.substr(begin, length);
  if (filePath == "Strona/")
    filePath += "index.html";
  return filePath;
}

std::string HTTPServer::formAnswer(std::string filePath)
{
  std::string answer = "HTTP/1.0 ";

  try
  {
    std::vector<char> file = readFile(filePath.c_str());
    answer += "200 OK\r\n";
    answer += "Content-Type: ";

    std::string type = filePath.substr(filePath.find(".") + 1);
    if (type == "html")
      answer += "text/html; charset=UTF-8";
    else if (type == "jpg")
      answer += "image/jpeg";
    else if (type == "js")
      answer += "application/javascript";
    else
      answer += "application/octet-stream";

    answer += "\r\n";
    answer += "Content-Length: " + std::to_string(file.size()) + "\r\n\r\n";
    for (char l : file)
      answer += l;
    answer += "\r\n\r\n";
  }
  catch (std::ios_base::failure)
  {
    answer += "404\r\n\r\n";
  }

  return answer;
}

void HTTPServer::closeConnection()
{
  close(clients[currentClientIndex].fd);
  clients.erase(clients.begin() + currentClientIndex);
  query = "";
  contentLength = 0;
  contentLeft = 100;
}

// function throws an exception ios_base::failure if file does not exist (or something goes wrong)
std::vector<char> HTTPServer::readFile(const char *path)
{
  using namespace std;

  vector<char> result;
  ifstream file(path, ios::in | ios::binary | ios::ate);
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  result.resize(file.tellg(), 0);
  file.seekg(0, ios::beg);
  file.read(result.data(), result.size());
  file.close();

  return result;
}