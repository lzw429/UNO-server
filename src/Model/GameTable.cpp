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
    // players.resize((unsigned long) PLAYERMAX);

    // todo  this->cardStack = dealer.shuffle();
    // todo  dealer.spreadOut(players);
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

Player *GameTable::getPlayer(const string &username) {
    for (auto player:players) {
        if (player->getUsername() == username)
            return player;
    }
    return nullptr;
}

Player *GameTable::getPlayer(int i) {
    if (i >= players.size())
        return nullptr;
    return *(players.begin() + i);
}

void GameTable::removePlayer(const string &username) {
    for (auto i = players.begin(); i != players.end(); i++) {
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
