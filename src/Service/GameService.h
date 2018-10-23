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
#include "../Util/OnlineUtil.h"

using namespace std;

class GameService : public GameConstants {
private:
    static vector<GameTable> gameTables;
public:
    static vector<GameTable> &getGameTables();

    static string getPlayersJson(GameTable &gameTable);

    static void process_rq(const vector<string> &request, int fd); // 处理请求
    static void unicastGameTables(int fd); // 发送游戏房间 uno02 hall\r\n\r\nContent
    static void enterRoom(string username, string roomNum, int fd); // 进入游戏房间 uno02 enterroom username roomNumber
    static void broadcastRoomStatus(int room); // 广播房间状态
    static void broadcastRoomStatus(string roomNum); // 广播房间状态
    static void quitRoom(string username, int fd); // 退出游戏房间 uno02 quitroom username
    static void gameStart(int room); // 游戏对局初始化
    static void drawCard(string username, string roomNum); // uno02 drawcard username roomNum
    static void sayUNO(string username, string roomNum); // uno02 sayuno username roomnum
    static void remainCard(int room); // uno02 remaincard remainCardNum
    static void playCard(const vector<string> &request); // uno02 playcard username roomnum cardnum
    static void setError(string error, int room); // uno02 seterror error
    static bool checkGameOver(int room); // 检查游戏是否结束 uno02 gameover
    static void setWildColor(int room, int color); // uno02 wildcolor colornum
};

#endif //UNOSERVER_GAMESERVICE_H
