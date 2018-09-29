//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_USERSERVICE_H
#define UNOSERVER_USERSERVICE_H

#include <string>
#include <vector>
#include <unordered_map>

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
        login(request[2], fd);
    }
}

void UserService::login(string username, int fd) {

}


#endif //UNOSERVER_USERSERVICE_H
