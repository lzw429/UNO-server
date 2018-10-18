//
// Created by syh on 10/15/18.
//
#include "CardDeck.h"

/**
 * 构造函数
 */
CardDeck::CardDeck() {
    shuffle();
}

vector<UNOCard> CardDeck::shuffle() {
    UNOCards.clear();
    int cardNumber = 0;
    for (int color:UNO_COLORS) {
        // 创建 76 张数字牌，除了 0 每个数字 2 张
        for (int num:UNO_NUMBERS) {
            int i = 0;
            do {
                UNOCard unoCard(cardNumber++, color, UNOCard::NUMBER, num);
                UNOCards.push_back(unoCard);
                i++;
            } while (num != 0 && i < 2);
        }

        // 创建 24 张动作牌，每种颜色每种动作类型 2 张
        for (int type: actionTypes) {
            for (int i = 0; i < 2; i++) {
                UNOCard unoCard(cardNumber++, color, UNOCard::ACTION, type);
                UNOCards.push_back(unoCard);
            }
        }
    }
    // 创建万能牌
    for (int type:wildTypes) {
        for (int i = 0; i < 4; i++) {
            UNOCard unoCard(cardNumber++, UNOCard::BLACK, type);
            UNOCards.push_back(unoCard);
        }
    }
    printTime();
    printf("CardDeck: %d cards has been created\n", cardNumber);
    return UNOCards;
}

const vector<UNOCard> &CardDeck::getUNOCards() const {
    return UNOCards;
}