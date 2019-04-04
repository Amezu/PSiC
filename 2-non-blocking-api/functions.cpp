#include "functions.h"

void check(int status, const char* name) {
  if(status == -1) {
    perror(name);
    exit(1);
  }
}

void set_ipv4(sockaddr_in& sin, int port, const char* ip) {
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(port);
  sin.sin_addr.s_addr = inet_addr(ip);  
}