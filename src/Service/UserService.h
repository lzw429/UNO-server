//
// Created by syh on 9/27/18.
//
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

#ifndef UNOSERVER_USERSERVICE_H
#define UNOSERVER_USERSERVICE_H

#include "../Model/User.h"
#include "../Util/OnlineUtil.h"

using namespace std;

class UserService {
private:
    static unordered_map<string, User> users; // 由用户名映射到文件描述符的 map

public:
    static void process_rq(const vector<string> &request, int fd); // 处理请求
    static int login(const string &username, int fd);   // 登入
    static void logout(int fd); // 登出
    static unordered_map<string, User> &getUsers(); // getter
    static void setUsers(const unordered_map<string, User> &users); // setter
};

#endif //UNOSERVER_USERSERVICE_H
