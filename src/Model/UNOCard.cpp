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