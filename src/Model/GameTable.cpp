//
// Created by syh on 10/15/18.
//
#include "GameTable.h"

// 静态变量
// status 常量
const int GameTable::IDLE = 0;
const int GameTable::WAITING = 1;
const int GameTable::GAMING = 2;

// mode 常量
const int GameTable::ONLINE = 2;
const int GameTable::PLAYERMAX = 2;

GameTable::GameTable() : status(IDLE), mode(-1) {
}

const vector<Player *> &GameTable::getPlayers() const {
    return players;
}

void GameTable::addPlayer(Player *player) {
    if (players.size() < PLAYERMAX) {
        players.push_back(player);
    } else {
        printTime();
        printf("GameTable: this table is already full\n");
    }
}

string GameTable::getPlayerName(int i) {
    if (i >= players.size())
        return "";
    return players[i]->getUsername();
}

Player *GameTable::getPlayerByUsername(const string &username) {
    for (auto player:players) {
        if (player->getUsername() == username)
            return player;
    }
    return nullptr;
}

Player *GameTable::getNextPlayer(const string &username) {
    int i = 0;
    for (; i < players.size(); i++) {
        if (players[i]->getUsername() == username)
            break;
    }
    return players[(i + 1) % players.size()];
}

Player *GameTable::getPlayerByIndex(int i) {
    if (i >= players.size())
        return nullptr;
    return *(players.begin() + i);
}

void GameTable::removePlayer(const string &username) {
    for (auto i = players.begin(); i != players.end(); i++) {
        // todo 非法访问？
        if ((*i) != nullptr && (*i)->getUsername() == username) {
            delete (*i);
            players.erase(i);
            break;
        }
    }
}

void GameTable::removePlayer(const Player *player) {
    for (auto i = players.begin(); i != players.end(); i++) {
        if ((*i) == player) {
            delete (*i);
            players.erase(i);
            break;
        }
    }
}

int GameTable::getStatus() const {
    return this->status;
}

void GameTable::setStatus(int status) {
    this->status = status;
}

Dealer &GameTable::getDealer() {
    return dealer;
}

/**
 * 设定出牌顺序
 * 向玩家发牌
 */
void GameTable::gameStart() {
    if (status != GAMING)
        return;
    // 设定出牌顺序
    int firstPlayer = (randomNumber(GameTable::PLAYERMAX - 1));
    players[firstPlayer]->setIsMyTurn(true);
    // 从牌桌向玩家发牌
    dealer.shuffle();
    dealer.spreadOut(players);
}

UNOCard GameTable::drawCard(string username) {
    Player *player = this->getPlayerByUsername(username);
    if (!player->isIsMyTurn()) {
        printTime();
        printf("GameTable: game turn error\n");
    }
    // 模型层 取牌
    return player->obtainOneCard(dealer.giveOneCard());
}

string GameTable::nextTurn(string username) {
    this->getPlayerByUsername(username)->setIsMyTurn(false);

    Player *nextPlayer = getNextPlayer(username);
    if (nextPlayer != nullptr) {
        nextPlayer->setIsMyTurn(true);
        return nextPlayer->getUsername();
    }
}
