//
// Created by syh on 10/15/18.
//
#include "Dealer.h"

stack<UNOCard> Dealer::shuffle() { // 洗牌
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
        cardStack.push(card);
    }
    return cardStack;
}

void Dealer::spreadOut(vector<Player *> &players) {
    // 给每位玩家发牌
    for (int i = 1; i <= firstHand; i++) {
        for (Player *p:players) {
            p->obtainCard(cardStack.top());
            cardStack.pop();
        }
    }
}

const UNOCard Dealer::getACard() {
    const UNOCard ret = cardStack.top();
    cardStack.pop();
    return ret;
}