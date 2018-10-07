//
// Created by syh on 9/26/18.
//

#ifndef UNOSERVER_CARDDECK_H
#define UNOSERVER_CARDDECK_H

#include <vector>
#include <string>
#include "UNOCard.h"
#include "GameConstants.h"

using namespace std;

class CardDeck : GameConstants {
private:
    vector<UNOCard> UNOCards;
public:
    const vector<UNOCard> &getUNOCards() const {
        return UNOCards;
    }

    CardDeck() { // 构造函数
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
        printf("CardDeck: %d cards has been created\n", cardNumber);
    }
};


#endif //UNOSERVER_CARDDECK_H
