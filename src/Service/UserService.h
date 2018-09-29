//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_USERSERVICE_H
#define UNOSERVER_USERSERVICE_H

#include <string>
#include <vector>

using namespace std;

class UserService {
public:
    void process_rq(const vector<string> &request); // 处理请求
    void login(string username); // 登录
};

void UserService::process_rq(const vector<string> &request) {
    if (request[1] == "login") {
        // uno01 login username
        login(request[2]);
    }
}

void UserService::login(string username) {

}


#endif //UNOSERVER_USERSERVICE_H
