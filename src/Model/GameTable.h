//
// Created by syh on 9/29/18.
//

#ifndef UNOSERVER_GAMETABLE_H
#define UNOSERVER_GAMETABLE_H

#include <stack>
#include "Player.h"
#include "CardDeck.h"
#include "Dealer.h"

using namespace std;

class GameTable { // 游戏桌
private:
    vector<Player *> players;   // 玩家
    int status;                 // 状态
    int mode;                   // 模式
    stack<UNOCard> cardStack;   // 牌栈
    Dealer dealer;              // 发牌者

public:
    // status 常量
    static const int IDLE;
    static const int WAITING;
    static const int GAMING;

    // mode 常量
    static const int ONLINE;
    static const int PLAYERMAX;

    GameTable();

    explicit GameTable(int mode); // todo 构造函数

    const vector<Player *> &getPlayers() const;

    void addPlayer(Player *player);

    Player *getPlayer(const string &username);

    Player *getPlayer(int i);

    string getPlayerName(int i);

    void removePlayer(const string &username);

    void removePlayer(const Player *player);

    int getStatus() const;

    void setStatus(int status);
};

#endif //UNOSERVER_GAMETABLE_H
