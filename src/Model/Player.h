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
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::property_tree;

class Player : public User { // 玩家
private:
    bool isMyTurn;
    bool saidUNO;
    vector<UNOCard> myCards;
    int playedCards; // 已打牌数

public:
    /**
     * 构造函数
     * @param user 用户
     */
    Player(User user)
            : User(user), isMyTurn(false), saidUNO(false), playedCards(0) {
    }

    /**
     * 构造函数
     * @param username 用户名
     */
    Player(string username) : isMyTurn(false),
                              saidUNO(false), playedCards(0) { // 构造方法
        this->username = std::move(username); // 继承
    }

    /**
     * 拷贝构造函数
     * @param player 被拷贝对象
     */
    Player(const Player &player)
            : isMyTurn(player.isMyTurn), saidUNO(player.saidUNO),
              playedCards(player.playedCards) {
        for (auto unoCard:player.myCards) {
            myCards.push_back(unoCard);
        }
    }

    /* getter & setter */

    bool isIsMyTurn() const {
        return isMyTurn;
    }

    void setIsMyTurn(bool isMyTurn) {
        this->isMyTurn = isMyTurn;
    }

    bool isSaidUNO() const {
        return saidUNO;
    }

    void setSaidUNO(bool saidUNO) {
        this->saidUNO = saidUNO;
    }

    vector<UNOCard> &getMyCards()  {
        return myCards;
    }

    void setMyCards(const vector<UNOCard> &myCards) {
        this->myCards = myCards;
    }

    int getPlayedCards() const {
        return playedCards;
    }

    void setPlayedCards(int playedCards) {
        this->playedCards = playedCards;
    }

    /**
       * 获取一张牌
       * @param card 来自发牌者的牌
       */
    UNOCard obtainOneCard(UNOCard card) {
        myCards.push_back(card);
        return card;
    }

/**
 * 删除打出的牌，并计数
 * @param cardNumber 牌号
 */
    UNOCard playCard(int cardNumber) {
        UNOCard unoCard;
        for (auto i = myCards.begin(); i != myCards.end(); i++) {
            if ((*i).getNumber() == cardNumber) {
                unoCard = *i;
                myCards.erase(i);
                playedCards++;
                break;
            }
        }
        return unoCard;
    }

    void sayUNO() {
        // todo
    }

    string toJson() {
        ptree playerJson, myCardsJson;
        playerJson.put("username", username);
        playerJson.put("isMyTurn", isMyTurn);

        for (UNOCard &unoCard:myCards) {
            ptree unoCardJson = unoCard.toJsonElement();
            myCardsJson.push_back(make_pair("", unoCardJson)); // JSON 数组
        }
        playerJson.add_child("myCards", myCardsJson);

        stringstream ss;
        write_json(ss, playerJson, false);
        return ss.str();
    }
};

#endif //UNOSERVER_PLAYER_H
