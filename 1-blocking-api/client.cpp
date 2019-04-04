#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

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

	int status;
	status = connect(sockfd, (struct sockaddr*) &sin, sizeof(sin));
	if(status == -1) {
		perror("connect");
		exit(1);
	}
	else
		std::cout << "connect status = " << status << std::endl;

	char buff[100] = "Dziala!";

	status = send(sockfd, &buff, sizeof(buff), 0);
	if(status == -1) {
		perror("send");
		exit(1);
	}
	else
		std::cout << "send status = " << status << std::endl;
	
	status = recv(sockfd, &buff, sizeof(buff), 0);
	if(status == -1) {
		perror("recv");
		exit(1);
	}
	else
		std::cout << "recv status = " << status << std::endl;
	std::cout << buff << "\n";

	close(sockfd);
}