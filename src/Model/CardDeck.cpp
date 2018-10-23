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
        // 创建 76 张数字牌，除了 0 每个数字 2 张，每个数字有 4 种颜色
        for (int value:UNO_NUMBERS) {
            int i = 0;
            do {
                UNOCard unoCard(cardNumber++, color, UNOCard::NUMBER, value);
                UNOCards.push_back(unoCard);
                i++;
            } while (value != 0 && i < 2);
        }

        // 创建 24 张动作牌，每种颜色每种动作类型 2 张
        for (int value: actionTypes) {
            for (int i = 0; i < 2; i++) {
                UNOCard unoCard(cardNumber++, color, UNOCard::ACTION, value);
                UNOCards.push_back(unoCard);
            }
        }
    }
    // 创建万能牌
    for (int value:wildTypes) {
        for (int i = 0; i < 4; i++) {
            UNOCard unoCard(cardNumber++, UNOCard::BLACK, UNOCard::WILD, value);
            UNOCards.push_back(unoCard);
        }
    }
    printTime();
    printf("CardDeck: %d cards has been created\n", cardNumber);
    fprintf(logFile, "CardDeck: %d cards has been created\n", cardNumber);
    fflush(logFile);
    return UNOCards;
}

const vector<UNOCard> &CardDeck::getUNOCards() const {
    return UNOCards;
}