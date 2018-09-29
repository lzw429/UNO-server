//
// Created by syh on 9/29/18.
//

#ifndef UNOSERVER_PLAYER_H
#define UNOSERVER_PLAYER_H

#include <string>
#include <vector>
#include "UNOCard.h"

using namespace std;

class Player { // 玩家
private:
    string username;
    bool isMyTurn;
    bool saidUNO;
    vector<UNOCard> myCards;

    int playedCards;

};


#endif //UNOSERVER_PLAYER_H
