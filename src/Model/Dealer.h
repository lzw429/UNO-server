//
// Created by syh on 10/6/18.
//

#ifndef UNOSERVER_DEALER_H
#define UNOSERVER_DEALER_H

#include <random>
#include "Player.h"
#include "CardDeck.h"
#include "UNOCard.h"

using namespace std;

class Dealer : GameConstants { // 发牌者
private:
    CardDeck cardDeck; // 一副牌
    stack<UNOCard> cardStack;

public:
    stack<UNOCard> shuffle() { // 洗牌
        vector<UNOCard> deck = cardDeck.getUNOCards();
        vector<UNOCard> shuffledCards;

        while (!deck.empty()) {
            int totalCards = static_cast<int>(deck.size());

            // 获取随机位置
            default_random_engine random;
            int pos = static_cast<int>(random()) % totalCards;

            UNOCard randomCard = deck[pos];
            deck.erase(deck.begin() + pos);
            shuffledCards.push_back(randomCard);
        }

        for (const UNOCard &card:shuffledCards) {
            cardStack.push_back(card);
        }
        return cardStack;
    }

    void spreadOut(vector<Player> &players) {
        // 给每位玩家发牌
        for (int i = 1; i <= firstHand; i++) {
            for (Player &p:players) {
                p.obtainCard(cardStack.top());
                cardStack.pop();
            }
        }
    }

    const UNOCard *getCard() {
        if (cardStack.size() < 1) {
            printf("Dealer: CardStack has no card now")
            return nullptr;
        }
        const UNOCard *ret = cardStack.top();
        cardStack.pop();
        return ret;
    }
};


#endif //UNOSERVER_DEALER_H