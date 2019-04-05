#include "TCPSocket.h"
#include <stdio.h>
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <fcntl.h>

TCPSocket::TCPSocket()
{
  fd = socket(AF_INET, SOCK_STREAM, 0);

  if (fd == -1)
  {
    perror("TCPsocket");
    exit(1);
  }

  letReuseAddress();
}

TCPSocket::~TCPSocket()
{
  close(fd);
}

void TCPSocket::letReuseAddress()
{
  int option = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

void TCPSocket::makeNonBlocking()
{
  int flags = fcntl(fd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(fd, F_SETFL, flags);
}

void TCPSocket::listenOnAddress(int port, const char *ip)
{
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = inet_addr(ip);
  unsigned int size = sizeof(sin);
  if (bind(fd, (sockaddr *)&sin, size) == -1)
  {
    perror("bind");
    exit(1);
  }
  if (listen(fd, 1) == -1)
  {
    perror("listen");
    exit(1);
  }
}