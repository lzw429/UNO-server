//
// Created by syh on 9/29/18.
//

#ifndef UNOSERVER_GAMETABLE_H
#define UNOSERVER_GAMETABLE_H


#include "Player.h"
#include "CardDeck.h"

class GameTable { // 游戏桌
private:
    vector<Player> players;
    CardDeck cardDeck;
    bool started;

public:
    const vector<Player> &getPlayers() const {
        return players;
    }

    void setPlayers(const vector<Player> &players) {
        GameTable::players = players;
    }

    const CardDeck &getCardDeck() const {
        return cardDeck;
    }

    void setCardDeck(const CardDeck &cardDeck) {
        GameTable::cardDeck = cardDeck;
    }

    bool isStarted() const {
        return started;
    }

    void setStarted(bool started) {
        GameTable::started = started;
    }

    string getPlayerName(int i) {
        if (i + 1 > players.size())
            return nullptr;
        return players[i].getUsername();
    }
};


#endif //UNOSERVER_GAMETABLE_H
