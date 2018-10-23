#ifndef SOCKLIB_H
#define SOCKLIB_H

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
#include <queue>
#include <sys/time.h>
#include "../Service/GameService.h"
#include "TimeUtil.h"
#include "../Service/UserService.h"
#include "../Model/Player.h"
#include "../Model/Request.h"

#define HOSTLEN 256
#define BACKLOG 1

#define TV_SEC 3 // timeval 秒数
#define TV_USEC 0 // timeval 毫秒数
#define FD_SIZ 25 // size of fd set


using namespace std;

extern pthread_mutex_t fdSetMutex; // 用于 fdSet
extern pthread_mutex_t requestsMutex; // 用于消息队列
extern pthread_mutex_t bytesSentMutex; // 用于统计已发送字节数
extern pthread_cond_t processStart;

extern time_t server_started;

extern int server_bytes_sent;
extern int server_requests;

extern int sock;

extern bitset<FD_SIZ> fdSet; // 文件描述符位图
extern queue<Request> requests; // 请求队列

// 函数声明

void setup(pthread_attr_t *attrp);

void *listenClientsThread(void *); // 监听客户端的线程

void process_rq(char *request, int fd);

void *processThread(void *); // 处理请求的线程

void closeSocket(int);

void process_msg(int fd);

void threadCreateRet(int); // 线程创建结果处理

int getMaxFd();


int make_server_socket(int portnum);

int make_server_socket_q(int, int);

int unicast(int fd, const char *msg);

int multicast(const vector<Player *> &players, char *msg);

int broadcast(const char *msg);

#endif// SOCKLIB_H