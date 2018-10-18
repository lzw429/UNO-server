#include <cstdio>
#include <queue>
#include <sys/time.h>
#include "Util/socklib.h"
#include "Model/Request.h"
#include "Service/GameService.h"
#include <boost/algorithm/string.hpp>

#define TV_SEC 3 // timeval 秒数
#define TV_USEC 0 // timeval 毫秒数
#define FD_SIZ 25 // size of fd set

using namespace std;
using namespace boost;

time_t server_started;

int server_bytes_sent;
int server_requests;

// 互斥量
pthread_mutex_t fdSetMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t requestsMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t processStart = PTHREAD_COND_INITIALIZER;

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
    int fd;
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

    printTime();
    printf("UNOServer: main thread has started\n");

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
        printTime();
        printf("UNOServer: accept client #%d\n", fd);
        printTime();
        printf("UNOServer: server request number: %d\n", server_requests);
        fdSet.set(fd);
        pthread_mutex_unlock(&fdSetMutex);
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
    fd_set readFds;
    struct timeval timeout;
    // 设置超时值
    timeout.tv_sec = TV_SEC;
    timeout.tv_usec = TV_USEC;

    int retVal; // 从 select 返回
    int maxFd = 0;

    printTime();
    printf("UNOServer: listening thread has started\n");

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

    while (true) {
        // 创建需要监听的文件描述符列表
        maxFd = getMaxFd();    // 该方法中有 lock 与 unlock
        pthread_mutex_lock(&fdSetMutex);
        FD_ZERO(&readFds);
        for (int i = 0; i < maxFd + 1; i++) {
            if (fdSet.test(i))
                FD_SET(i, &readFds);
        }
        pthread_mutex_unlock(&fdSetMutex);

        // 等待输入
        retVal = select(maxFd + 1, &readFds, nullptr, nullptr, &timeout);
        if (retVal == -1) {
            perror("select");
            continue;
        }
        if (retVal > 0) {
            // 对每个文件描述符检查位
            pthread_mutex_lock(&fdSetMutex);
            for (int i = 0; i < FD_SIZ; i++) {
                if (fdSet.test(i) && FD_ISSET(i, &readFds)) {
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

    if (n == -1) {
        printTime();
        perror("read from socket");
    }
    if (n > 0) {
        printTime();
        printf("Receive from client #%d: request = %s", fd, buf);
        // 将消息放入队列
        string content = buf;
        Request request(content, fd);
        pthread_mutex_lock(&requestsMutex); // 加锁
        requests.push(request);
        pthread_cond_signal(&processStart); // 告知请求处理线程
        pthread_mutex_unlock(&requestsMutex); // 释放
    } else if (n == 0) {
        // 客户端离线, fdSetMutex 已加锁
        server_requests--;
        UserService::logout(fd);
        fdSet.reset(fd);
        close(fd);
        printTime();
        printf("Receive from client #%d: disconnect\n", fd);
    }
}

/**
 * 处理请求的线程
 * @param ptr
 * @return
 */
void *processThread(void *ptr) {
    printTime();
    printf("UNOServer: process request thread has started\n");
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        pthread_mutex_lock(&requestsMutex); // 加锁
        while (requests.empty())
            pthread_cond_wait(&processStart, &requestsMutex); // 有消息时唤醒
        while (!requests.empty()) {
            Request request = requests.front();
            requests.pop();
            process_rq(request.getContentChar(), request.getFd()); // 处理客户端请求
        }
        pthread_mutex_unlock(&requestsMutex); // 释放
    }
#pragma clang diagnostic pop
}

void process_rq(char *request, int fd) {
    if (strcmp(request, "") == 0)
        return;
    vector<string> splitStr;
    string rq_str = request;
    split(splitStr, rq_str, boost::is_any_of(" "));
    if (splitStr[0] == "uno01")
        UserService::process_rq(splitStr, fd);
    else if (splitStr[0] == "uno02")
        GameService::process_rq(splitStr, fd);
    else {
        throw "process_rq: request split exception";
    }
}

void closeSocket(int signum) {
#ifndef CLOSESOCKET
    if (sock != -1) {
        shutdown(sock, SHUT_RDWR);
        printTime();
        printf("UNOServer: Socket shutdown\n");
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
    int ret = 0;
    pthread_mutex_lock(&fdSetMutex);
    for (int i = FD_SIZ - 1; i >= 0; --i) {
        if (fdSet.test(i)) {
            ret = i;
            break;
        }
    }
    pthread_mutex_unlock(&fdSetMutex);
    return ret;
}

