#include <utility>

//
// Created by syh on 9/26/18.
//

#ifndef UNOSERVER_UNOCARD_H
#define UNOSERVER_UNOCARD_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost::property_tree;

class UNOCard {
private:
    int number; // 每副牌中的编号
    int color;
    int type;
    int value;

public:
    /* 常量 */
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

    /* getter & setter */
    int getNumber() const {
        return number;
    }

    void setNumber(int number) {
        UNOCard::number = number;
    }

    int getCardColor() const {
        return color;
    }

    void setCardColor(int cardColor) {
        UNOCard::color = cardColor;
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

    /* 构造函数 */
    UNOCard(int number, int color, int type)
            : number(number), color(color), type(type), value(0) {
    }

    UNOCard(int number, int color, int type, int value)
            : number(number), color(color), type(type),
              value(value) {}

    /**
     * 返回 UNOCard 的 JSON 表示
     * @return JSON 形式的字符串
     */
    string toJson() {
        ptree cardJson;
        cardJson.put("number", number);
        cardJson.put("color", color);
        cardJson.put("type", type);
        cardJson.put("value", value);

        stringstream ss;
        write_json(ss, cardJson, false);
        return ss.str();
    }
};

#endif //UNOSERVER_UNOCARD_H
