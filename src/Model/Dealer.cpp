//
// Created by syh on 10/15/18.
//
#include "Dealer.h"

/**
  * 洗牌
  * @return 洗好的一副完整的牌
  */
stack<UNOCard> Dealer::shuffle() {
    vector<UNOCard> deck = cardDeck.getUNOCards(); // 一副完整的牌
    vector<UNOCard> shuffledCards;

    while (!deck.empty()) {
        int totalCards = (int) (deck.size());

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

/**
  * 发牌者给每位玩家发牌
  * @param players
  */
void Dealer::spreadOut(vector<Player *> &players) {
    for (int i = 1; i <= firstHand; i++) { // firstHand 是初始牌数
        for (Player *p:players) {
            p->obtainCard(cardStack.top()); // 玩家从牌栈里拿一张
            cardStack.pop();
        }
    }
}

UNOCard Dealer::getACard() {
    UNOCard ret = this->cardStack.top();
    this->cardStack.pop();
    return ret;
}