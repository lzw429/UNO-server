//
// Created by syh on 9/26/18.
//

#ifndef UNOSERVER_CARDDECK_H
#define UNOSERVER_CARDDECK_H

#include <vector>
#include <string>
#include "UNOCard.h"
#include "GameConstants.h"
#include "../Util/TimeUtil.h"

using namespace std;

class CardDeck : GameConstants {
private:
    vector<UNOCard> UNOCards;
public:
    const vector<UNOCard> &getUNOCards() const;

    vector<UNOCard> shuffle();

    CardDeck(); // 构造函数
};


#endif //UNOSERVER_CARDDECK_H
