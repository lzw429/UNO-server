#include <cstdio>
#include <sys/time.h>
#include "Util/socklib.h"
#include "Service/GameService.h"
#include <boost/algorithm/string.hpp>
#include <csignal>

#define TV_SEC 3
#define TV_USEC 0

using namespace std;
using namespace boost;

time_t server_started;

int server_bytes_sent;
int server_requests;
TimeUtil timeUtil;


// 互斥量
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;;

// 函数声明

void setup(pthread_attr_t *attrp);

void *listenClients(void *);

void *handle_call(void *fdptr);

void process_rq(char *request, int fd);

void closeSocket(int);

void process_msg(int fd);

int sock;

vector<int> fdSet;

int main(int ac, char *av[]) {
    int fd = -1;
    int *fdptr;
    pthread_t worker;
    pthread_attr_t attr;

    if (ac == 1) {
        fprintf(stderr, "usage: UNOServer portnum\n");
        exit(1);
    }

    sock = make_server_socket(atoi(av[1]));
    if (sock == -1) {
        perror("making socket");
        exit(2);
    }

    setup(&attr);
    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    printf("UNOServer has started\n");

    signal(SIGINT, closeSocket);

    int ret = pthread_create(&worker, &attr, listenClients, nullptr);
    if (ret != 0) {
        perror("UNOServer: listening thread create failed");
        exit(2);
    }

    // 主循环，接收请求，以新线程处理请求
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        fd = accept(sock, nullptr, nullptr);
        if (fd == -1) {
            perror("UNOServer: socket connection failed");
            continue;
        }

        pthread_mutex_lock(&mutex);
        server_requests++;
        pthread_mutex_unlock(&mutex);

        fdSet.push_back(fd);
        printf("UNOServer: accept client #%d\n", fd);

    }
#pragma clang diagnostic pop
}

// 函数实现

void setup(pthread_attr_t *attrp) {
    pthread_attr_init(attrp);
    pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED);

    time(&server_started);
    server_requests = 0; // todo 统计数据
    server_bytes_sent = 0;
}

/**
 * 使用 select 监听客户端
 * @return
 */
void *listenClients(void *ptr) {
    fd_set readfds;
    struct timeval timeout;
    int retval; // 从 select 返回
    int maxFd;

    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    printf("UNOServer: listening thread has started\n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    while (true) {
        // 创建需要监听的文件描述符列表
        maxFd = 5;
        FD_ZERO(&readfds);
        //  pthread_mutex_lock(&mutex);
        for (int fd:fdSet) {
            FD_SET(fd, &readfds);
        }
        // pthread_mutex_unlock(&mutex);

        // 设置超时值
        timeout.tv_sec = TV_SEC;
        timeout.tv_usec = TV_USEC;

        // 等待输入
        retval = select(maxFd, &readfds, nullptr, nullptr, &timeout);
        if (retval == -1) {
            perror("select");
            continue;
        }
        if (retval > 0) {
            // 对每个文件描述符检查位
            // pthread_mutex_lock(&mutex);
            for (int fd:fdSet) {
                if (FD_ISSET(fd, &readfds)) {
                    process_msg(fd);
                }
            }
            // pthread_mutex_unlock(&mutex);
        }
    }

#pragma clang diagnostic pop
}

void process_msg(int fd) {
    char buf[BUFSIZ];
    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    printf("Receive from client #%d: request = %s", fd, buf);
    fflush(stdout);
    int n = read(fd, buf, BUFSIZ);
    if (n == -1) {
        perror("read from socket");
    }
    write(1, buf, n); // stdout
    write(1, "\n", 1);
    process_rq(buf, fd); // 处理客户端请求
}

/**
 * 处理请求，由线程执行
 * @param fdptr 文件描述符指针
 * @return
 */
void *handle_call(void *fdptr) {
    // TODO 处理完成后销毁线程?
    // TODO 客户端退出后关闭 socket
//    FILE *fpin;
    char request[BUFSIZ];
    int fd;
    TimeUtil timeUtil;

    fd = *(int *) fdptr;
    free(fdptr); // 由参数获取文件描述符

    while (true) {
        bzero(request, BUFSIZ);
        ssize_t len = recv(fd, request, BUFSIZ, 0);
        if (len < 0) {
            printf("Receive data failed\n");
            exit(1);
        } else if (len > 0) {
            printf("[%s] ", timeUtil.getTimeInMillis().c_str());
            printf("Receive from client #%d, len = %d: request = %s",
                   fd, (int) len, request);
            process_rq(request, fd); // 处理客户端请求
        }
    }
}

void process_rq(char *request, int fd) {
    if (strcmp(request, "") == 0)
        return;
    UserService userService;
    GameService gameService;
    vector<string> splitStr;
    string rq_str = request;
    split(splitStr, rq_str, boost::is_any_of(" "));
    if (splitStr[0] == "uno01")
        userService.process_rq(splitStr, fd);
    else if (splitStr[0] == "uno02")
        gameService.process_rq(splitStr, fd);
    else {
        throw "process_rq: request split exception";
    }
}

void closeSocket(int signum) {
#ifndef CLOSESOCKET
    if (sock != -1) {
        shutdown(sock, SHUT_RDWR);
        printf("Socket shutdown\n");
    }
#endif
}