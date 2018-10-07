#include <utility>

//
// Created by syh on 9/26/18.
//

#ifndef UNOSERVER_UNOCARD_H
#define UNOSERVER_UNOCARD_H

#include <string>

using namespace std;

class UNOCard {
private:
    int number; // 每副牌中的编号
    int cardColor;
    int type;
    int value;

public:
    // color
    static const int RED = 0;
    static const int BLUE = 1;
    static const int GREEN = 2;
    static const int YELLOW = 3;
    static const int BLACK = 4;

    // type
    static const int NUMBER = 1;
    static const int ACTION = 2;
    static const int WILD = 3;

    // action type
    static const int REVERSE = 0;
    static const int SKIP = 1;
    static const int DRAW2PLUS = 2;

    // wild type
    static const int COLOR_PICKER = 0;
    static const int DRAW4PLUS = 1;

    // getter & setter
    int getNumber() const {
        return number;
    }

    void setNumber(int number) {
        UNOCard::number = number;
    }

    int getCardColor() const {
        return cardColor;
    }

    void setCardColor(int cardColor) {
        UNOCard::cardColor = cardColor;
    }

    int getType() const {
        return type;
    }

    void setType(int type) {
        UNOCard::type = type;
    }

    int getValue() const {
        return value;
    }

    void setValue(int value) {
        UNOCard::value = value;
    }

    // 构造函数
    UNOCard(int number, int cardColor, int type) : number(number), cardColor(cardColor), type(type), value(0) {
    }

    UNOCard(int number, int cardColor, int type, int value) : number(number), cardColor(cardColor), type(type),
                                                              value(value) {}
};

#endif //UNOSERVER_UNOCARD_H
