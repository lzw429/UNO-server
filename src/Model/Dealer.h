//
// Created by syh on 10/6/18.
//

#ifndef UNOSERVER_DEALER_H
#define UNOSERVER_DEALER_H

#include <random>
#include <stack>
#include "Player.h"
#include "CardDeck.h"
#include "UNOCard.h"

using namespace std;

class Dealer : GameConstants { // 发牌者
private:
    CardDeck cardDeck; // 一副牌
    stack<UNOCard> cardStack;

public:
    stack<UNOCard> shuffle();

    void spreadOut(vector<Player *> &players);

    const UNOCard getACard();
};

#endif //UNOSERVER_DEALER_H