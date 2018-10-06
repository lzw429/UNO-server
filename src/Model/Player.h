#include <utility>

//
// Created by syh on 9/29/18.
//

#ifndef UNOSERVER_PLAYER_H
#define UNOSERVER_PLAYER_H

#include <string>
#include <vector>
#include "UNOCard.h"
#include "User.h"

using namespace std;

class Player : public User { // 玩家
private:
    bool isMyTurn;
    bool saidUNO;
    vector<UNOCard> myCards;
    int playedCards; // 已打牌数

public:
    Player(User user) : User(user) {
    }

    Player(string username) : isMyTurn(false),
                              saidUNO(false), playedCards(0) { // 构造方法
        this->username = std::move(username); // 继承
    }

    void obtainCard(UNOCard card) {
        myCards.push_back(card);
    }

    bool isIsMyTurn() const {
        return isMyTurn;
    }

    void setIsMyTurn(bool isMyTurn) {
        Player::isMyTurn = isMyTurn;
    }

    bool isSaidUNO() const {
        return saidUNO;
    }

    void setSaidUNO(bool saidUNO) {
        Player::saidUNO = saidUNO;
    }

    const vector<UNOCard> &getMyCards() const {
        return myCards;
    }

    void setMyCards(const vector<UNOCard> &myCards) {
        Player::myCards = myCards;
    }

    int getPlayedCards() const {
        return playedCards;
    }

    void setPlayedCards(int playedCards) {
        Player::playedCards = playedCards;
    }

    void playCard(string username); // 打牌
    void drawCard(string username);  // 抽牌
    void sayUNO(string username); // 说 UNO
};


#endif //UNOSERVER_PLAYER_H
