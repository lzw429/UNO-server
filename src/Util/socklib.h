#include <cstdio>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctime>
#include <strings.h>
#include <string>
#include <cstring>

#define HOSTLEN 256
#define BACKLOG 1

#ifndef SOCKLIB_H
#define SOCKLIB_H

#include "TimeUtil.h"
#include "../Service/UserService.h"

using namespace std;

// 声明
int make_server_socket(int portnum);

int make_server_socket_q(int, int);

int unicast(int fd, const char *msg);

int broadcast(const char *msg);

#endif// SOCKLIB_H