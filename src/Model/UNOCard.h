//
// Created by syh on 9/26/18.
//

#ifndef UNOSERVER_UNOCARD_H
#define UNOSERVER_UNOCARD_H

#include <string>

using namespace std;


class UNOCard {
private:
    string cardColor;
    string type; // "NUMBER" "ACTION" "WILD"
    string value;
public:
    const string &getCardColor() const {
        return cardColor;
    }

    void setCardColor(const string &cardColor) {
        UNOCard::cardColor = cardColor;
    }

    const string &getType() const {
        return type;
    }

    void setType(const string &type) {
        UNOCard::type = type;
    }

    const string &getValue() const {
        return value;
    }

    void setValue(const string &value) {
        UNOCard::value = value;
    }

    UNOCard(const string &cardColor, const string &type, const string &value) : cardColor(cardColor), type(type),
                                                                                value(value) {}
};


#endif //UNOSERVER_UNOCARD_H
