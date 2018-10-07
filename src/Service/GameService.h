#include <utility>

//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMESERVICE_H
#define UNOSERVER_GAMESERVICE_H

#include <string>
#include <vector>
#include "UserService.h"
#include "../Model/GameTable.h"
#include "../socklib.h"

using namespace std;

class GameService {
private:
    static vector<GameTable> gameTables;
public:
    void process_rq(const vector<string> &request, int fd); // 处理请求
    void sendGameTables(int fd); // 发送游戏房间 uno02 hall\r\n
    void enterRoom(string username, string roomNum, int fd); // 进入游戏房间 uno02 enterroom username roomNumber
    void initCardInfo(string username, string roomNum, int fd); // 初始化卡牌信息
    void quitRoom(string username, int fd); // 退出游戏房间 uno02 quitroom username
};

vector<GameTable> GameService::gameTables(10); // static

void GameService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "hall\r\n") { // 请求大厅数据
        sendGameTables(fd);
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
void GameService::sendGameTables(int fd) {
    string msg = "uno02 hall\r\n\r\n";
    try {
        for (GameTable gameTable:gameTables) {
            msg = msg + gameTable.getPlayer(0)->getUsername() + ","
                  + gameTable.getPlayer(1)->getUsername() + ","
                  + to_string(gameTable.getStatus()) + "\r\n";
        }
        sendMsg(fd, nullptr, msg.c_str());
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
    if (room > gameTables.size() || room < 0) {
        printf("GameService: room number exception");
        return;
    }
    UserService userService;
    auto users = userService.getUsers();
    User &user = users[username]; // 找到已登录的用户
    // todo 判断是否可进入该房间
    user.setRoomNum(room); // 设置房间号
    Player player(user);
    gameTables[room].addPlayer(player); // 进入房间

    // 向客户端返回进入房间成功
    char *msg = new char[64];
    sprintf(msg, "uno02 enterroom %d 1\r\n", room);
    sendMsg(fd, nullptr, msg);
    printf("GameService: %s has entered room #%s", username.c_str(), roomNum.c_str());
    delete[]msg;
}

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
 * 对局开始发送初始的卡牌信息
 * @param username 用户名
 * @param roomNum  房间号
 * @param fd 文件描述符
 */
void GameService::initCardInfo(string username, string roomNum, int fd) {
    stringstream ss(roomNum);
    int room = -1;
    ss >> room;
    if (room == -1)return;
    auto players = gameTables[room].getPlayers();

    char *msg = new char[64];
    int cardSize0 = (int) players[0]->getMyCards().size();
    int cardSize1 = (int) players[1]->getMyCards().size();
    sprintf(msg, "uno02 cardnum %d %d\r\n", cardSize0, cardSize1);
    sendMsg(fd, nullptr, msg);
    delete[]msg;

    msg = new char[1024]; // 传输一个玩家手中的牌应该足够
    string content = "";
    for (auto card:gameTables[room].getPlayer(username)->getMyCards()) {
        // todo content
        content += to_string(card.getCardColor()) + " "
                   + to_string(card.getType()) + " "
                   + to_string(card.getValue()) + "\r\n";
    }
    sprintf(msg, "uno02 cardinfo\r\n\r\n%s", content);
    sendMsg(fd, nullptr, msg);
    delete[]msg;
}

#endif //UNOSERVER_GAMESERVICE_H
