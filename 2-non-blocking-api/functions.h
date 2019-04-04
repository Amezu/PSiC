#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <stdio.h>

void check(int status, const char* name);
void set_ipv4(sockaddr_in& sin, int port, const char* ip);