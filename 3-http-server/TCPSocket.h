#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <netinet/in.h>

class TCPSocket
{
public:
  TCPSocket();
  ~TCPSocket();

  void makeNonBlocking();
  void listenOnAddress(int port, const char *ip);

  operator int() { return fd; }

private:
  void letReuseAddress();

  int fd;
  sockaddr_in sin;
  unsigned size;
};

#endif /*TCPSOCKET_H*/