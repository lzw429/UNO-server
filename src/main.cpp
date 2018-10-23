#include "Util/OnlineUtil.h"

using namespace std;
using namespace boost;


int main(int ac, char *av[]) {
    int fd;
    pthread_t listen; // 监听客户端
    pthread_t process;  // 处理请求
    pthread_attr_t attr;

    if (ac == 1) {
        fprintf(stderr, "usage: UNOServer portnum\n");
        fprintf(logFile, "usage: UNOServer portnum\n");
        fflush(logFile);
        exit(1);
    }

    sock = make_server_socket(atoi(av[1]));
    if (sock == -1) {
        perror("making socket");
        exit(2);
    }

    setup(&attr); // 设置线程属性 attr

    // 初始化统计数据 todo 统计
    time(&server_started);
    server_requests = 0;
    server_bytes_sent = 0;

    printTime();
    printf("UNOServer: main thread has started\n");
    fprintf(logFile, "UNOServer: main thread has started\n");
    fflush(logFile);

    int ret = pthread_create(&listen, &attr, listenClientsThread, nullptr); // 创建监听客户端的线程
    threadCreateRet(ret);
    ret = pthread_create(&process, &attr, processThread, nullptr); // 创建处理请求的线程
    threadCreateRet(ret);

    signal(SIGINT, SIG_INT_Response); // 处理信号
    signal(SIGPIPE, SIG_IGN);

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
        fprintf(logFile, "UNOServer: accept client #%d\n", fd);
        fflush(logFile);
        printTime();
        printf("UNOServer: server request number: %d\n", server_requests);
        fprintf(logFile, "UNOServer: server request number: %d\n", server_requests);
        fflush(logFile);
        fdSet.set(fd);
        pthread_mutex_unlock(&fdSetMutex);
    }
#pragma clang diagnostic pop
}
