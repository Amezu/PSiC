#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdio.h>

int main() 
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		perror("socket");
		exit(1);
	}
	else
		std::cout << "socket status = " << sockfd << std::endl;

	struct sockaddr_in sin;
	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");
	sin.sin_port = htons(65280);
	unsigned int size = sizeof(sin);

	char buffer[100];

	int status;
	status = bind(sockfd, (struct sockaddr*) &sin, size);
	if(status == -1) {
		perror("bind");
		exit(1);
	}
	// std::cout << "bind status = " << status << std::endl;

	status = listen(sockfd, 1);
	if(status == -1) {
		perror("listen");
		exit(1);
	}
	// std::cout << "listen status = " << status << std::endl;

	int receiver = accept(sockfd, (struct sockaddr*) &sin, &size);
	if(receiver == -1) {
		perror("accept");
		exit(1);
	}
	// std::cout << "accept status = " << receiver << std::endl;

	// struct sockaddr_in client;
	// memset(&client, 0, sizeof(client));
	// unsigned int client_len = sizeof(client);
	// getpeername(receiver, (struct sockaddr*) &client, &client_len);
	std::cout << "address=" << inet_ntoa(sin.sin_addr) << "\t";
	std::cout << "port=" << ntohs(sin.sin_port) << "\n";

	status = recv(receiver, buffer, sizeof(buffer), 0);
	if(status == -1) {
		perror("recv");
		exit(1);
	}
	// std::cout << "recv status = " << status << std::endl;
	
	std::cout << "from " <<  << "\treceived\t" << buffer << "\n";

	char msg[100] = "Re: ";
	strcat(msg, buffer);
	status = send(receiver, msg, sizeof(msg), 0);
	if(status == -1) {
		perror("send");
		exit(1);
	}
	// std::cout << "send status = " << status << std::endl;

	close(receiver);
	close(sockfd);
}