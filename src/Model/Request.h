#include <utility>

//
// Created by syh on 10/12/18.
//

#ifndef UNOSERVER_REQUEST_H
#define UNOSERVER_REQUEST_H

#include <string>

using namespace std;

class Request {
private:
    string content; // 请求内容
    int fd; // 文件描述符
public:
    /* getter & setter */
    const string &getContent() const {
        return content;
    }

    void setContent(const string &content) {
        Request::content = content;
    }

    int getFd() const {
        return fd;
    }

    void setFd(int fd) {
        Request::fd = fd;
    }

    /**
     * 构造函数
     * @param content 请求内容
     * @param fd 文件描述符
     */
    Request(string content, int fd) : content(std::move(content)), fd(fd) {}

    /**
     * 获得请求内容
     * @return 字符串格式的请求内容
     */
    char *getContentChar() {
        return (char *) content.c_str();
    }
};


#endif //UNOSERVER_REQUEST_H
