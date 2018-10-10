//
// Created by syh on 10/9/18.
//
#include "socklib.h"

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

int connect_to_server(char *host, int portnum) {
    int sock;
    struct sockaddr_in servadd; /* the number to call */
    struct hostent *hp; /* used to get number */
/** Step 1: Get a socket **/
    sock = socket(AF_INET, SOCK_STREAM, 0); /* get a line */
    if (sock == -1)
        return -1;
/** Step 2: connect to server **/
    bzero(&servadd, sizeof(servadd)); /* zero the address */
    hp = gethostbyname(host); /* lookup host's ip # */
    if (hp == nullptr)
        return -1;
    bcopy(hp->h_addr, (struct sockaddr *) &servadd.sin_addr, hp->h_length);
    servadd.sin_port = htons(portnum); /* fill in port number */
    servadd.sin_family = AF_INET; /* fill in socket type */
    if (connect(sock, (struct sockaddr *) &servadd, sizeof(servadd)) != 0)
        return -1;
    return sock;
}

/**
 * 向客户端发送消息
 * @param fd 文件描述符
 * @param msg 消息内容
 * @return 发送的字节数
 */
int unicast(int fd, const char *msg) {
    TimeUtil timeUtil;
    string msgStr = msg;
    int len = (int) send(fd, msg, msgStr.size(), 0);
    printf("[%s] ", timeUtil.getTimeInMillis().c_str());

    if (len >= 0) {
        printf("Server send to client #%d, len = %d: %s", len, fd, msg);
    } else {
        printf("Send send to client #%d exception : %s\n",
               fd, msg);
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