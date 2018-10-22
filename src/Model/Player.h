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
#include <boost/algorithm/string.hpp>

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
    Player(User user);

    /**
     * 构造函数
     * @param username 用户名
     */
    Player(string username);

    /**
     * 拷贝构造函数
     * @param player 被拷贝对象
     */
    Player(const Player &player);

    /* getter & setter */

    bool isIsMyTurn() const;

    void setIsMyTurn(bool isMyTurn);

    bool isSaidUNO() const;

    void setSaidUNO(bool saidUNO);

    vector<UNOCard> &getMyCards();

    void setMyCards(const vector<UNOCard> &myCards);

    int getPlayedCards() const;

    void setPlayedCards(int playedCards);

    /**
     * 获取一张牌
     * @param card 来自发牌者的牌
     * @return 获得的牌
     */
    UNOCard obtainOneCard(UNOCard card);

    /**
     * 删除打出的牌，并计数
     * @param cardNumber 牌号
     */
    UNOCard playCard(int cardNumber);

    string toJson();
};

#endif //UNOSERVER_PLAYER_H
