//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMECONSTANTS_H
#define UNOSERVER_GAMECONSTANTS_H

#include <string>
#include "UNOCard.h"

using namespace std;

class GameConstants { // 接口
public:
    const int firstHand = 8; // 初始牌数
    const int UNO_COLORS[4] = {UNOCard::RED, UNOCard::BLUE, UNOCard::GREEN, UNOCard::YELLOW};
    const int UNO_NUMBERS[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const int actionTypes[3] = {UNOCard::REVERSE, UNOCard::SKIP, UNOCard::DRAW2PLUS};
    const int wildTypes[2] = {UNOCard::COLOR_PICKER, UNOCard::DRAW4PLUS};
};

#endif //UNOSERVER_GAMECONSTANTS_H
