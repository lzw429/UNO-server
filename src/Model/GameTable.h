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
    vector<Player> players;     // 玩家
    int status;                 // 状态
    int mode;                   // 模式
    stack<UNOCard> cardStack;   // 牌栈
    Dealer dealer;              // 发牌者

public:
    // status 常量
    const int IDLE = 0;
    const int WAITING = 1;
    const int GAMING = 2;

    // mode 常量
    const int ONLINE = 2;

    explicit GameTable(const int mode) : mode(mode) { // 构造方法
        this->mode = mode;
        this->status = IDLE;
        this->cardStack = dealer.shuffle();
        dealer.spreadOut(players);
        players.resize(2);
    }

    const vector<Player> &getPlayers() const {
        return players;
    }

    void setPlayers(const vector<Player> &players) {
        GameTable::players = players;
    }

    void setPlayer(int i, const Player &player) {
        players[i] = player;
    }

    int getStatus() const {
        return status;
    }

    void setStatus(int status) {
        GameTable::status = status;
    }
};


#endif //UNOSERVER_GAMETABLE_H
