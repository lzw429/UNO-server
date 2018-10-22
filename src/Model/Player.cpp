//
// Created by syh on 10/22/18.
//
#include "Player.h"

Player::Player(User user)
        : User(user), isMyTurn(false), saidUNO(false), playedCards(0) {
}

Player::Player(string username) : isMyTurn(false),
                                  saidUNO(false), playedCards(0) { // 构造方法
    this->username = std::move(username); // 继承
}

Player::Player(const Player &player)
        : isMyTurn(player.isMyTurn), saidUNO(player.saidUNO),
          playedCards(player.playedCards) {
    for (auto unoCard:player.myCards) {
        myCards.push_back(unoCard);
    }
}

bool Player::isIsMyTurn() const {
    return isMyTurn;
}

void Player::setIsMyTurn(bool isMyTurn) {
    this->isMyTurn = isMyTurn;
}

bool Player::isSaidUNO() const {
    return saidUNO;
}

vector<UNOCard> &Player::getMyCards() {
    return myCards;
}

string Player::toJson() {
    ptree playerJson, myCardsJson;
    playerJson.put("username", username);
    playerJson.put("isMyTurn", isMyTurn);

    for (UNOCard &unoCard:myCards) {
        ptree unoCardJson = unoCard.toJsonElement();
        myCardsJson.push_back(make_pair("", unoCardJson)); // JSON 数组
    }
    if (myCards.empty()) {
        ptree emptyChildJson;
        myCardsJson.push_back(make_pair("", emptyChildJson));
    }
    playerJson.add_child("myCards", myCardsJson);

    stringstream ss;
    write_json(ss, playerJson, false);

    string ret = ss.str();
    boost::replace_all(ret, "[\"\"]", "[]");

    return ret;
}

void Player::setSaidUNO(bool saidUNO) {
    this->saidUNO = saidUNO;
}

void Player::setMyCards(const vector<UNOCard> &myCards) {
    this->myCards = myCards;
}

int Player::getPlayedCards() const {
    return playedCards;
}

void Player::setPlayedCards(int playedCards) {
    this->playedCards = playedCards;
}

UNOCard Player::obtainOneCard(UNOCard card) {
    myCards.push_back(card);
    return card;
}

UNOCard Player::playCard(int cardNumber) {
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
