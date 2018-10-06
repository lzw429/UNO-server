#include <utility>

//
// Created by syh on 10/6/18.
//

#ifndef UNOSERVER_USER_H
#define UNOSERVER_USER_H

#include <string>

using namespace std;

class User {
private:
    int fd; // 文件描述符
    int roomNum; // 所在房间号

protected:
    string username; // 用户名

public:
    User() : fd(0), roomNum(0) {}

    User(string username, int fd)
            : username(std::move(username)), fd(fd), roomNum(-1) {
        // 构造方法
    }

    const string &getUsername() const {
        return username;
    }

    void setUsername(const string &username) {
        User::username = username;
    }

    int getFd() const {
        return fd;
    }

    void setFd(int fd) {
        User::fd = fd;
    }

    int getRoomNum() const {
        return roomNum;
    }

    void setRoomNum(int roomNum) {
        User::roomNum = roomNum;
    }
};

#endif //UNOSERVER_USER_H
