//
// Created by syh on 10/9/18.
//
#include "OnlineUtil.h"

// 互斥量
pthread_mutex_t fdSetMutex = PTHREAD_MUTEX_INITIALIZER; // 用于 fdSet
pthread_mutex_t requestsMutex = PTHREAD_MUTEX_INITIALIZER; // 用于消息队列
pthread_mutex_t bytesSentMutex = PTHREAD_MUTEX_INITIALIZER; // 用于统计已发送字节数
pthread_cond_t processStart = PTHREAD_COND_INITIALIZER;

time_t server_started;

int server_bytes_sent;
int server_requests;

int sock;

bitset<FD_SIZ> fdSet; // 文件描述符位图
queue<Request> requests; // 请求队列

struct sockaddr_in saddr; /* build our address here */

int make_server_socket(int portnum) {
    return make_server_socket_q(portnum, BACKLOG);
}

int make_server_socket_q(int portnum, int backlog) {
    struct hostent *hp; /* this is part of our */
    char hostname[HOSTLEN]; /* address */
    int sock_id; /* the socket */
    sock_id = socket(PF_INET, SOCK_STREAM, 0); /* get a socket */
    if (sock_id == -1)
        return -1;
    /** 设置 socket 选项避免被占用的错误 **/
    int on = 1;
    if ((setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    /** 构建地址并将 socket 绑定 **/
    bzero((void *) &saddr, sizeof(saddr)); /* clear out struct */
    gethostname(hostname, HOSTLEN); /* where am I ? */
    hp = gethostbyname(hostname); /* get info about host */
/* fill in host part */
    bcopy((void *) hp->h_addr, (void *) &saddr.sin_addr, hp->h_length);
    saddr.sin_port = htons(portnum); /* fill in socket port */
    saddr.sin_family = AF_INET; /* fill in addr family */
    if (bind(sock_id, (struct sockaddr *) &saddr, sizeof(saddr)) != 0)
        return -1;
    /** 监听请求 **/
    if (listen(sock_id, backlog) != 0)
        return -1;
    return sock_id;
}

/**
 * 向单个客户端发送消息
 * @param fd 文件描述符
 * @param msg 消息内容
 * @return 发送的字节数
 */
int unicast(int fd, const char *msg) {
    string msgStr = msg;
    msgStr += "\f";
    int len = (int) send(fd, msgStr.c_str(), msgStr.size(), 0);

    pthread_mutex_lock(&bytesSentMutex);
    server_bytes_sent += len;
    pthread_mutex_unlock(&bytesSentMutex);

    if (len >= 0) {
        printTime();
        printf("Server: %d bytes in total; send to client #%d, len = %d: %s",
               server_bytes_sent, fd, len, msg);
    } else {
        printTime();
        printf("Send send to client #%d exception : %s\n", fd, msg);
    }
    return len;
}

/**
 * 向房间内玩家发送消息
 * @param players 房间内玩家
 * @param msg 消息
 * @return 发送的总字节数
 */
int multicast(const vector<Player *> &players, char *msg) {
    int len = 0;
    for (Player *roomReceiver:players) {
        // 向房间内所有玩家发送房间内各玩家信息
        len += unicast(roomReceiver->getFd(), msg);
    }
    return len;
}

/**
 * 向所有连接服务器的客户端发送消息
 * @param msg 消息内容
 * @return 发送的总字节数
 */
int broadcast(const char *msg) {
    UserService userService;
    auto users = userService.getUsers();
    int len = 0;
    string msgStr = msg;
    for (auto item:users) {
        int fd = item.second.getFd();
        len += unicast(fd, msg);
    }
    return len;
}


/**
 * 设置线程属性
 * @param attrp 线程属性
 */
void setup(pthread_attr_t *attrp) {
    pthread_attr_init(attrp);
    pthread_attr_setdetachstate(attrp, PTHREAD_CREATE_DETACHED); // 分离线程
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
