//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMECONSTANTS_H
#define UNOSERVER_GAMECONSTANTS_H


class GameConstants { // 接口
public:
    string UNO_COLORS[4] = {"red", "blue", "green", "yellow"};
    int UNO_NUMBERS[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    string actionTypes[3] = {"reverse", "skip", "draw2plus"};
    string wildTypes[2] = {"w_colorpicker", "w_draw4plus"};
};

#endif //UNOSERVER_GAMECONSTANTS_H
