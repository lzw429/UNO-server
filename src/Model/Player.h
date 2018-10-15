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

    /**
       * 获取一张牌
       * @param card 来自发牌者的牌
       */
    void obtainCard(UNOCard card) {
        myCards.push_back(card);
    }

    void playCard(int cardNumber) {
        // todo
    }

    void drawCard() {
        // todo
    }

    void sayUNO() {
        // todo
    }

    string toJson() {
        ptree playerJson, myCardsJson;
        playerJson.put("username", username);
        playerJson.put("isMyTurn", isMyTurn);

        for (UNOCard &unoCard:myCards) {
            ptree unoCardJson;
            unoCardJson.put("", unoCard.toJson());
            myCardsJson.push_back(make_pair("", unoCardJson));
        }
        playerJson.add_child("myCards", myCardsJson);

        stringstream ss;
        write_json(ss, playerJson, false);
        return ss.str();
    }
};

#endif //UNOSERVER_PLAYER_H
