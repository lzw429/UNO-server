//
// Created by syh on 10/16/18.
//

#include "UNOCard.h"

ptree UNOCard::toJsonElement() {
    ptree cardJson;
    cardJson.put("number", number);
    cardJson.put("color", color);
    cardJson.put("type", type);
    cardJson.put("value", value);
    return cardJson;
}

string UNOCard::toJson() {
    stringstream ss;
    write_json(ss, toJsonElement(), false);
    return ss.str();
}

UNOCard::UNOCard(int number, int color, int type, int value)
        : number(number), color(color), type(type),
          value(value) {}

UNOCard::UNOCard() {}

UNOCard::UNOCard(const UNOCard &unoCard) : number(unoCard.number), color(unoCard.color), type(unoCard.type),
                                           value(unoCard.value) {}
