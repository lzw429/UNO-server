//
// Created by syh on 10/9/18.
//
#include "UserService.h"

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
    logout(fd); // 退出该连接所有账户
    string msg = "uno01 login " + username;
    if (users.find(username) != users.end()) { // 该用户已注册
        msg += " 0\r\n";
        printTime();
        printf("UserService: user %s cannot be registered again\n", username.c_str());
        fprintf(logFile, "UserService: user %s cannot be registered again\n", username.c_str());
        fflush(logFile);
    } else { // 该用户未注册
        msg += " 1\r\n";
        printTime();
        printf("UserService: user %s got registered\n", username.c_str());
        fprintf(logFile, "UserService: user %s got registered\n", username.c_str());
        fflush(logFile);
        User user(username, fd);
        users[username] = user;
    }
    return unicast(fd, msg.c_str());
}

/**
 * 登出
 * @param fd 文件描述符
 */
void UserService::logout(int fd) {
    for (auto i = users.begin(); i != users.end(); ++i) {
        if (i->second.getFd() == fd) {
            int room = i->second.getRoomNum();
            auto &gameTables = GameService::getGameTables();
            GameService::quitRoom(i->second.getUsername(), fd); // 退出房间
            users.erase(i); // 删除用户
            break;
        }
    }
}

unordered_map<string, User> &UserService::getUsers() {
    return users;
}

void UserService::setUsers(const unordered_map<string, User> &users) {
    UserService::users = users;
}
