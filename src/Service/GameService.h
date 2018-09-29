//
// Created by syh on 9/27/18.
//

#ifndef UNOSERVER_GAMESERVICE_H
#define UNOSERVER_GAMESERVICE_H

#include <string>
#include <vector>
#include "../Model/GameTable.h"

using namespace std;

class GameService {
private:
    static vector<GameTable> gameTables;
public:
    void process_rq(const vector<string> &request); // 处理请求
    void playCard(string username); // 打牌
    void drawCard(string username);  // 抽牌
    void sayUNO(string username); // 说 UNO
};

void GameService::process_rq(const vector<string> &request) {

}


#endif //UNOSERVER_GAMESERVICE_H
