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
        for (string color:UNO_COLORS) {
            // 创建 76 张数字牌，除了 0 每个数字 2 张
            for (int num:UNO_NUMBERS) {
                int i = 0;
                do {
                    UNOCard unoCard(color, "NUMBER", to_string(num));
                    UNOCards.push_back(unoCard);
                    i++;
                } while (num != 0 && i < 2);
            }

            // 创建 24 张动作牌，每种颜色每种动作类型 2 张
            for (string type: actionTypes) {
                for (int i = 0; i < 2; i++) {
                    UNOCard unoCard(color, "ACTION", type);
                }
            }
        }
        // 创建万能牌
        for (string type:wildTypes) {
            for (int i = 0; i < 4; i++) {
                UNOCard unoCard("BLACK", type, "");
                UNOCards.push_back(unoCard);
            }
        }
    }
};


#endif //UNOSERVER_CARDDECK_H
