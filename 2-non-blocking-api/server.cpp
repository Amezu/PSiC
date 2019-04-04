#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>
#include "time_str.h"
#include "functions.h"

int main() {
  const int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  check(sockfd, "socket");

  // bind: Address already in use
  int option = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // asynchronicznosc (nieblokujace)
  int flags = fcntl(sockfd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, flags);

  sockaddr_in sin;
  set_ipv4(sin, 65285, "127.0.0.1");
  unsigned int size = sizeof(sin);

  check(bind(sockfd, (sockaddr*) &sin, size), "bind");
  check(listen(sockfd, 1), "listen");

  std::vector<pollfd> fds;
  fds.push_back({sockfd, POLLIN});
  std::string buffer;

  while(1) {
    check(poll(fds.data(), fds.size(), -1), "poll");

    for(unsigned i = 0; i < fds.size(); ++i) {
      if(fds[i].revents & POLLIN) {
        sockaddr_in client;
        memset(&client, 0, sizeof(client));
        unsigned client_len = sizeof(client);

        if(fds[i].fd == sockfd) {
          int receiver = accept(sockfd, (sockaddr*) &client, &client_len);
          check(receiver,"accept");
          fds.push_back({receiver, POLLIN | POLLOUT, 0});

          char host[100];
          check(getnameinfo((sockaddr*) &client, client_len, host, sizeof(host), NULL, 0, 0), "getnameinfo");

          std::cout << "NEW CONNECTION\taddress=" << inet_ntoa(client.sin_addr) << " (" << host << ")\t";
          std::cout << "port=" << ntohs(client.sin_port) << "\n";          
        }
        else {
          buffer.resize(100);

          int received = recv(fds[i].fd, const_cast<char*>(buffer.data()), buffer.length(), 0);
          check(received, "recv");
          getpeername(fds[i].fd, (sockaddr*) &client, &client_len);

          if(received == 0) {
            close(fds[i].fd);

            std::cout << "Closed connection " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\n";
            fds.erase(fds.begin()+i);
            break;
          }

          buffer.resize(received);

          std::cout << "from " << inet_ntoa(client.sin_addr) << ":" << ntohs(client.sin_port) << "\t";
          std::cout << "received\t" << buffer << std::endl;

          std::string time_str(getTime(buffer.data()));

          check(send(fds[i].fd, time_str.data(), time_str.length()+1, 0), "send");
        }
      }         
    }
  }

  close(sockfd);
}