//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMESERVICE_H
#define UNOSERVER_GAMESERVICE_H

#include <string>
#include <vector>
#include "../Model/GameTable.h"
#include "../socklib.h"

using namespace std;

class GameService {
private:
    static vector<GameTable> gameTables;
public:
    void process_rq(const vector<string> &request, int fd); // 处理请求
    void sendGameTables(int fd);
};

vector<GameTable> GameService::gameTables(10); // static

void GameService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "hall\r\n") { // 请求大厅数据
        sendGameTables(fd);
    }
}

void GameService::sendGameTables(int fd) {
    string msg = "uno02 hall\r\n\r\n";
    for (GameTable gameTable:gameTables) {
        msg = msg + gameTable.getPlayerName(0) + ","
              + gameTable.getPlayerName(1) + ","
              + to_string(gameTable.getStatus()) + "\r\n";
    }
    sendMsg(fd, nullptr, msg.c_str());
}

#endif //UNOSERVER_GAMESERVICE_H
