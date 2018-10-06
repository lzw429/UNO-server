//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_USERSERVICE_H
#define UNOSERVER_USERSERVICE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "../socklib.h"
#include "../Model/User.h"

using namespace std;

class UserService {
private:
    static unordered_map<string, User> users; // 由用户名映射到文件描述符的 map

public:
    void process_rq(const vector<string> &request, int fd); // 处理请求
    int login(const string &username, int fd);   // 登录
    static const unordered_map<string, User> &getUsers(); // getter
    static void setUsers(const unordered_map<string, User> &users); // setter
};

// 静态变量声明
unordered_map<string, User> UserService::users;

/**
 * 请求控制
 * @param request 客户端请求
 * @param fd 文件描述符
 */
void UserService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "login") {
        // uno01 login username\r\n
        login(request[2].substr(0, request[2].size() - 2), fd);
    }
}

/**
 * 处理用户登录
 * @param username 用户名
 * @param fd 文件描述符
 * @return 发送的字节数
 */
int UserService::login(const string &username, int fd) {
    string msg = "uno01 login " + username;
    if (users.find(username) != users.end()) { // 该用户已注册
        msg += " 0\r\n";
        printf("UserService: user %s cannot be registered again\n", username.c_str());
    } else { // 该用户未注册
        msg += " 1\r\n";
        printf("UserService: user %s got registered\n", username.c_str());
        User user(username, fd);
        users[username] = user;
    }
    return sendMsg(fd, nullptr, msg.c_str());
}

const unordered_map<string, User> &UserService::getUsers() {
    return users;
}

void UserService::setUsers(const unordered_map<string, User> &users) {
    UserService::users = users;
}

#endif //UNOSERVER_USERSERVICE_H
