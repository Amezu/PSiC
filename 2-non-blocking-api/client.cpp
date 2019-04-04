#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include <stdlib.h>
#include "functions.h"

int main(int argc, char *argv[]) 
{
  if(argc != 5) {
    std::cerr << "Wrong number of arguments\nExample of use:\n./client 4242 \"%H:%M:%S\" 2 5\n";
    exit(1);
  }

  const int port = strtol(argv[1], NULL, 0);
  const std::string format = argv[2] + '\0';
  const int secsToWait = strtol(argv[3], NULL, 0);
  const int queries = strtol(argv[4], NULL, 0);

  const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd, "socket");

  sockaddr_in my_sin;
  set_ipv4(my_sin, port, "127.0.0.1");
  unsigned int size = sizeof(my_sin);

  // bind: Address already in use
  int jeden = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &jeden, sizeof(jeden));

  check(bind(sockfd, (sockaddr*) &my_sin, size), "bind");

  // asynchronicznosc (nieblokujace)
  int flags = fcntl(sockfd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);

  sockaddr_in sin;
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(65285);
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");

  if(connect(sockfd, (sockaddr*) &sin, sizeof(sin)) == -1 && errno != 115) {
    perror("connect");
    exit(1);
  }

  std::vector<pollfd> fds;
  fds.push_back({sockfd, POLLOUT | POLLIN});

  check(poll(fds.data(), fds.size(), -1), "poll");

  for(int i=0; i<queries; ++i) {
    if(fds[0].revents & POLLOUT)
      check(send(sockfd, format.data(), format.length()+1, 0), "send");
    
    while(1) {
      check(poll(fds.data(), fds.size(), -1), "poll");

      if(fds[0].revents & POLLIN) {
        std::string time_str;
        time_str.resize(100);

        int status = recv(sockfd, const_cast<char*>(time_str.data()), time_str.length(), 0);
        check(status, "recv");

        time_str.resize(status);
        std::cout << time_str << "\n";
        sleep(secsToWait);
        break;
      }
    }
  }

  close(sockfd);
}
