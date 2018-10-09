#include <utility>

//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMESERVICE_H
#define UNOSERVER_GAMESERVICE_H

#include <cstdio>
#include <string>
#include <vector>
#include "UserService.h"
#include "../Model/GameTable.h"
#include "../Util/socklib.h"

using namespace std;

class GameService {
private:
    static vector<GameTable> gameTables;
public:
    void process_rq(const vector<string> &request, int fd); // 处理请求
    void unicastGameTables(int fd); // 发送游戏房间 uno02 hall\r\n\r\nContent
    void enterRoom(string username, string roomNum, int fd); // 进入游戏房间 uno02 enterroom username roomNumber
    void broadcastRoomStatus(string roomNum); // 广播房间状态
    void quitRoom(string username, int fd); // 退出游戏房间 uno02 quitroom username
};

vector<GameTable> GameService::gameTables(10); // static

void GameService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "hall\r\n") { // 请求大厅数据
        unicastGameTables(fd);
    } else if (request[1] == "enterroom") { // 请求进入房间
        enterRoom(request[2], request[3], fd);
    } else if (request[1] == "quitroom") { // 请求退出房间
        quitRoom(request[2], fd);
    }
}

/**
 * 发送房间信息
 * @param fd 文件描述符
 */
void GameService::unicastGameTables(int fd) {
    string msg = "uno02 hall\r\n\r\n";
    try {
        for (GameTable gameTable:gameTables) {
            Player *player0 = gameTable.getPlayer(0);
            Player *player1 = gameTable.getPlayer(1);
            string username0 = (player0 != nullptr) ? player0->getUsername() : "";
            string username1 = (player1 != nullptr) ? player1->getUsername() : "";
            msg = msg + username0 + "," + username1 + "," + to_string(gameTable.getStatus()) + "\r\n";
        }
        unicast(fd, msg.c_str());
    } catch (exception e) {
        printf("%s\nGameService: send game tables exception\n", e.what());
    }
}

/**
 * 客户请求进入房间，向客户返回进入结果
 * @param username 用户名
 * @param roomNum 房间号
 * @param fd 文件描述符
 */
void GameService::enterRoom(string username, string roomNum, int fd) {
    int room = stoi(roomNum);
    UserService userService;
    char *msg = new char[64];

    if (room > gameTables.size() || room < 0) {
        printf("GameService: room number exception");
        return;
    }
    auto users = userService.getUsers();
    User &user = users[username]; // 找到已登录的用户
    // 判断是否可进入该房间
    GameTable gameTable;
    if (gameTables[room].getPlayers().size() == gameTable.PLAYERMAX) {
        // 房间已满
        sprintf(msg, "uno02 enterroom %d 0\r\n", room);
        unicast(fd, msg);
        delete[]msg;
        return;
    }
    user.setRoomNum(room); // 设置房间号
    Player *player = new Player(user);
    gameTables[room].addPlayer(player); // 进入房间

    // 向客户端返回进入房间成功

    sprintf(msg, "uno02 enterroom %d 1\r\n", room);
    unicast(fd, msg);
    printf("GameService: user %s has entered room #%s", username.c_str(), roomNum.c_str());
    delete[]msg;
    broadcastRoomStatus(roomNum); // 广播该房间状态
}

/**
 * 请求退出房间
 * @param username 用户名
 * @param fd 文件描述符
 */
void GameService::quitRoom(string username, int fd) {
    UserService userService;
    auto users = userService.getUsers();
    auto user_i = users.find(username); // 找到已登录的用户
    if (user_i == users.end()) {
        printf("GameService: this user does not exist");
        return;
    }
    User &user = users[username];
    int room = user.getRoomNum(); // 获得房间号
    user.setRoomNum(-1);
    gameTables[room].removePlayer(username); // 退出房间
    printf("GameService: %s has quit room #%d", username.c_str(), room);
}

/**
 * 发送指定房间内的玩家列表
 * @param roomNum 房间号
 * @param fd 文件描述符
 */
void GameService::broadcastRoomStatus(string roomNum) {
    int room = stoi(roomNum);
    stringstream msg;
    msg << "uno02 roomstatus " << room << " ";
    auto players = gameTables[room].getPlayers();
    if (players.size()) {
        for (auto player:players) {
            msg << player->getUsername() << ",";
        }
    }
    msg << gameTables[room].getStatus() << "\r\n";
    string msg_str = msg.str();
    broadcast(msg_str.c_str());
}

// todo 退出房间

#endif //UNOSERVER_GAMESERVICE_H
