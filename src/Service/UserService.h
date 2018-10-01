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

using namespace std;

class UserService {
private:
    static unordered_map<string, int> clients; // 由用户名映射到文件描述符的 map
public:
    void process_rq(const vector<string> &request, int fd); // 处理请求
    void login(string username, int fd); // 登录
};

void UserService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "login") {
        // uno01 login username
        login(request[2].substr(0, request[2].size() - 2), fd);
    }
}

unordered_map<string, int> UserService::clients;

void UserService::login(string username, int fd) {
    string msg = "uno01 login " + username + "\r\n";
    if (clients.find(username) != clients.end()) { // 该用户已注册
        msg += " 0";
        printf("UserService: user %s has been registered\n", username.c_str());
    } else { // 该用户未注册
        msg += " 1";
        printf("UserService: user %s got registered\n", username.c_str());
        clients[username] = fd;
    }
    sendMsg(fd, nullptr, msg.c_str());
}

#endif //UNOSERVER_USERSERVICE_H
