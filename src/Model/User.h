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
    /**
     * 构造方法
     */
    User() : fd(0), roomNum(0) {}

    /**
     * 构造方法
     * @param username 用户名
     * @param fd 文件描述符
     */
    User(string username, int fd)
            : username(std::move(username)), fd(fd), roomNum(-1) {
        // 构造方法
    }

    /**
     * 拷贝构造方法
     * @param user 被拷贝对象
     */
    User(const User &user) :
            fd(user.fd), roomNum(user.roomNum), username(user.username) {
    }

    /* getter & setter */
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
