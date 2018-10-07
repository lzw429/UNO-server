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
    const int IDLE = 0;
    const int WAITING = 1;
    const int GAMING = 2;

    // mode 常量
    const int ONLINE = 2;
    const int PLAYERMAX = 2;

    GameTable();

    explicit GameTable(const int mode);

    const vector<Player *> &getPlayers() const;

    void addPlayer(Player &player);

    Player *getPlayer(const string &username);

    Player *getPlayer(int i);

    string getPlayerName(int i);

    void removePlayer(const string &username);

    void removePlayer(const Player *player);

    int getStatus() const;

    void setStatus(int status);
};

GameTable::GameTable() : status(0), mode(-1) {

}

GameTable::GameTable(const int mode) : mode(mode) { // 构造方法
    this->mode = mode;
    this->status = IDLE;
    this->cardStack = dealer.shuffle();
    dealer.spreadOut(players);
    players.resize((unsigned long) PLAYERMAX);
}

const vector<Player *> &GameTable::getPlayers() const {
    return players;
}

void GameTable::addPlayer(Player &player) {
    if (players.size() < PLAYERMAX) {
        players.push_back(&player);
    } else {
        printf("GameTable: this table is already full");
    }
}

string GameTable::getPlayerName(int i) {
    if (i >= players.size())
        return "";
    return players[i]->getUsername();
}

Player *GameTable::getPlayer(const string &username) {
    for (auto player:players) {
        if (player->getUsername() == username)
            return player;
    }
    return nullptr;
}

Player *GameTable::getPlayer(int i) {
    if (i > players.size())
        return nullptr;
    return players[i];
}

void GameTable::removePlayer(const string &username) {
    for (auto i = players.begin(); i != players.end(); i++) {
        if ((*i)->getUsername() == username) {
            players.erase(i);
        }
    }
}

void GameTable::removePlayer(const Player *player) {
    for (auto i = players.begin(); i != players.end(); i++) {
        if ((*i) == player) {
            players.erase(i);
        }
    }
}

int GameTable::getStatus() const {
    return status;
}

void GameTable::setStatus(int status) {
    GameTable::status = status;
}
#endif //UNOSERVER_GAMETABLE_H
