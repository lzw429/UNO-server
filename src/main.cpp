#include <cstdio>
#include <queue>
#include <sys/time.h>
#include "Util/socklib.h"
#include "Model/Request.h"
#include "Service/GameService.h"
#include <boost/algorithm/string.hpp>
#include <csignal>

#define TV_SEC 3
#define TV_USEC 0
#define FD_SIZ 25

using namespace std;
using namespace boost;

time_t server_started;

int server_bytes_sent;
int server_requests;
TimeUtil timeUtil;

// 互斥量
pthread_mutex_t fdSetMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t requestsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t requestCome = PTHREAD_COND_INITIALIZER;

// 函数声明

void setup(pthread_attr_t *attrp);

void *listenClientsThread(void *); // 监听客户端的线程

void *handle_call(void *fdptr);

void process_rq(char *request, int fd);

void *processThread(void *); // 处理请求的线程

void closeSocket(int);

void process_msg(int fd);

void threadCreateRet(int); // 线程创建结果处理

int getMaxFd();

int sock;

bitset<FD_SIZ> fdSet; // 文件描述符位图
queue<Request> requests; // 请求队列

int main(int ac, char *av[]) {
    int fd = -1;
    int *fdptr;
    pthread_t listen; // 监听客户端
    pthread_t process;  // 处理请求
    pthread_attr_t attr;
    // todo setup process

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

    // 初始化统计数据 todo 统计
    time(&server_started);
    server_requests = 0;
    server_bytes_sent = 0;

    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    printf("UNOServer has started\n");

    signal(SIGINT, closeSocket); // 处理信号
    signal(SIGPIPE, SIG_IGN);

    int ret = pthread_create(&listen, &attr, listenClientsThread, nullptr); // 创建监听客户端的线程
    threadCreateRet(ret);
    ret = pthread_create(&process, &attr, processThread, nullptr); // 创建处理请求的线程
    threadCreateRet(ret);

    // 主循环，接收请求，以新线程处理请求
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        fd = accept(sock, nullptr, nullptr);
        if (fd == -1) {
            perror("UNOServer: socket connection failed");
            continue;
        }

        pthread_mutex_lock(&fdSetMutex);
        server_requests++;
        fdSet.set(fd);
        pthread_mutex_unlock(&fdSetMutex);

        printf("[%s] ", timeUtil.getTimeInMillis().c_str());
        printf("UNOServer: accept client #%d\n", fd);
    }
#pragma clang diagnostic pop
}

// 函数实现

void setup(pthread_attr_t *attrp) {
    pthread_attr_init(attrp);
    pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED);
}

/**
 * 使用 select 监听客户端的线程
 * @return
 */
void *listenClientsThread(void *ptr) {
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
        pthread_mutex_lock(&fdSetMutex);
        maxFd = getMaxFd();
        FD_ZERO(&readfds);
        for (int i = 0; i < FD_SIZ; i++) {
            if (fdSet.test(i))
                FD_SET(i, &readfds);
        }
        pthread_mutex_unlock(&fdSetMutex);

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
            pthread_mutex_lock(&fdSetMutex);
            for (int i = 0; i < FD_SIZ; i++) {
                if (fdSet.test(i) && FD_ISSET(i, &readfds)) {
                    process_msg(i);
                }
            }
            pthread_mutex_unlock(&fdSetMutex);
        }
    }

#pragma clang diagnostic pop
}

void process_msg(int fd) {
    char buf[BUFSIZ];
    bzero(buf, BUFSIZ);
    fflush(stdout);
    int n = (int) read(fd, buf, BUFSIZ);
    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    if (n == -1) {
        perror("read from socket");
    }
    if (n > 0) {
        printf("Receive from client #%d: request = %s", fd, buf);
        // 将消息放入队列
        string content = buf;
        Request request(content, fd);
        pthread_mutex_lock(&requestsMutex);
        requests.push(request);
        pthread_mutex_unlock(&requestsMutex);
    } else if (n == 0) {
        // 客户端离线
        pthread_mutex_lock(&fdSetMutex);
        server_requests--;
        fdSet.reset(fd);
        pthread_mutex_unlock(&fdSetMutex);

        printf("Receive from client #%d: disconnect\n", fd);
    }
}

/**
 * 处理请求的线程
 * @param ptr
 * @return
 */
void *processThread(void *ptr) {
    printf("[%s] ", timeUtil.getTimeInMillis().c_str());
    printf("UNOServer: process request thread has started\n");
    while (true) {
        // todo 有消息时唤醒
        pthread_cond_wait(&requestCome, &requestsMutex);
        if (!requests.empty()) {
            Request request = requests.front();
            requests.pop();
            process_rq(request.getContentChar(), request.getFd()); // 处理客户端请求
        }
    }
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

void threadCreateRet(int ret) {
    if (ret != 0) {
        perror("UNOServer: listening thread create failed");
        exit(2);
    }
}

int getMaxFd() {
    pthread_mutex_lock(&fdSetMutex);
    for (int i = FD_SIZ - 1; i >= 0; --i) {
        if (fdSet.test(i) == true)
            return i;
    }
    pthread_mutex_unlock(&fdSetMutex);
}
