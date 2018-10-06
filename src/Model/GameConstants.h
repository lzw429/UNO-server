//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMECONSTANTS_H
#define UNOSERVER_GAMECONSTANTS_H

#include <string>

using namespace std;

class GameConstants { // 接口
public:
    const int firstHand = 8; // 初始牌数
    const string UNO_COLORS[4] = {"red", "blue", "green", "yellow"};
    const int UNO_NUMBERS[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    const string actionTypes[3] = {"reverse", "skip", "draw2plus"};
    string wildTypes[2] = {"w_colorpicker", "w_draw4plus"};
};

#endif //UNOSERVER_GAMECONSTANTS_H
