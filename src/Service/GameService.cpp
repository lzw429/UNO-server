//
// Created by syh on 10/14/18.
//
#include "GameService.h"
// 静态变量声明

vector<GameTable> GameService::gameTables(10); // static

// 方法

void GameService::process_rq(const vector<string> &request, int fd) {
    if (request[1] == "hall\r\n") { // 请求大厅数据
        unicastGameTables(fd);
    } else if (request[1] == "enterroom") { // 请求进入房间
        enterRoom(request[2], request[3], fd);
    } else if (request[1] == "quitroom") { // 请求退出房间
        quitRoom(request[2], fd);
    } else if (request[1] == "drawcard") { // 请求抽牌
        drawCard(request[2], request[3]);
    } else if (request[1] == "playcard") { // 请求打牌
        playCard(request);
    } else if (request[1] == "sayuno") { // 请求说 UNO
        sayUNO(request[2], request[3]);
    }
}

/**
 * 发送房间信息
 * @param fd 文件描述符
 */
void GameService::unicastGameTables(int fd) {
    string msg = "uno02 hall\r\n\r\n";
    try {
        for (GameTable gameTable:gameTables) {
            Player *player0 = gameTable.getPlayerByIndex(0);
            Player *player1 = gameTable.getPlayerByIndex(1);
            string username0 = (player0 != nullptr) ? player0->getUsername() : "";
            string username1 = (player1 != nullptr) ? player1->getUsername() : "";
            msg += username0 + "," + username1 + "," + to_string(gameTable.getStatus()) + "\r\n";
        }
        unicast(fd, msg.c_str());
    } catch (exception &e) {
        printf("%s\nGameService: send game tables exception\n", e.what());
    }
}

/**
 * 客户请求进入房间，向客户返回进入结果
 * @param username 用户名
 * @param roomNum 房间号
 * @param fd 文件描述符
 */
void GameService::enterRoom(string username, string roomNum, int fd) {
    int room = stoi(roomNum);
    char *msg = new char[64];
    bool start = false;

    if (room > gameTables.size() || room < 0) {
        printTime();
        printf("GameService: room number exception");
        return;
    }
    unordered_map<string, User> &users = UserService::getUsers();
    User &user = users[username]; // 找到已登录的用户
    GameTable &gameTable = gameTables[room];
    // 判断是否可进入该房间
    if (gameTable.getStatus() == GameTable::GAMING) {
        // 房间已满
        sprintf(msg, "uno02 enterroom %d 0\r\n", room);
        unicast(fd, msg); // 单播登录结果
        delete[]msg;
        return;
    }
    if (user.getRoomNum() != -1) { // 退出之前已进入的房间
        quitRoom(username, fd);
    }
    user.setRoomNum(room); // 设置房间号
    Player *player = new Player(user);
    gameTable.addPlayer(player); // 进入房间

    // 修改房间状态
    if (gameTable.getPlayers().size() == GameTable::PLAYERMAX) { // 开始游戏
        gameTable.setStatus(GameTable::GAMING);
        start = true;
    } else { // 房间继续等待其他玩家
        gameTable.setStatus(GameTable::WAITING);
    }

    // 向客户端返回消息

    sprintf(msg, "uno02 enterroom %d 1\r\n", room);
    unicast(fd, msg); // 单播登录结果
    printTime();
    printf("GameService: user %s has entered room #%s", username.c_str(), roomNum.c_str());
    delete[]msg;
    broadcastRoomStatus(roomNum); // 广播该房间状态
    if (start)
        gameStart(room); // 开始游戏对局
}

/**
 * 请求退出房间
 * @param username 用户名
 * @param fd 文件描述符
 */
void GameService::quitRoom(string username, int fd) {
    unordered_map<string, User> &users = UserService::getUsers();
    auto user_i = users.find(username); // 找到已登录的用户

    if (user_i == users.end()) {
        printTime();
        printf("GameService: user %s (client #%d) does not exist", username.c_str(), fd);
        return;
    }
    User &user = users[username];
    int room = user.getRoomNum(); // 获得房间号
    user.setRoomNum(-1);
    GameTable &gameTable = gameTables[room];
    gameTable.removePlayer(username); // 退出房间

    // 修改房间状态
    if (gameTable.getPlayers().empty()) { // 房间为空闲
        gameTable.setStatus(GameTable::IDLE);
        // todo 重置房间内的牌

    } else { // 房间继续等待其他玩家
        gameTable.setStatus(GameTable::WAITING);
    }
    printTime();
    printf("GameService: player %s (client #%d) has quit room #%d\n", username.c_str(), fd, room);
    broadcastRoomStatus(room);
}

/**
 * 发送指定房间内的玩家列表
 * @param roomNum 房间号
 * @param fd 文件描述符
 */
void GameService::broadcastRoomStatus(int room) {
    stringstream msg;
    GameTable &gameTable = gameTables[room];
    const vector<Player *> &players = gameTable.getPlayers();
    auto playerSize = players.size();

    msg << "uno02 roomstatus " << room << " ";
    for (int i = 0; i < playerSize && i < GameTable::PLAYERMAX; i++) {
        string username;
        username = "";
        if (players[i] != nullptr) {
            username = players[i]->getUsername();
        }
        msg << username << ",";
    }
    for (int i = 0; i < GameTable::PLAYERMAX - playerSize; i++) {
        msg << ",";
    }
    msg << gameTable.getStatus() << "\r\n";
    string msg_str = msg.str();
    broadcast(msg_str.c_str());
}

/**
 * 发送指定房间内的玩家列表
 * @param roomNum 房间号
 * @param fd 文件描述符
 */
void GameService::broadcastRoomStatus(string roomNum) {
    int room = stoi(roomNum);
    broadcastRoomStatus(room);
}

vector<GameTable> &GameService::getGameTables() {
    return gameTables;
}

/**
 * 游戏对局开始,以 JSON 传输 GameTable 信息
 * @param room 房间号
 */
void GameService::gameStart(int room) {
    GameTable &gameTable = gameTables[room];
    UNOCard firstCard;
    gameTable.gameStart(firstCard);

    // 生成消息中的 JSON
    const vector<Player *> &players = gameTable.getPlayers();
    string playersJson = getPlayersJson(gameTable); // 空格等已处理
    string firstCardJson = firstCard.toJson(); // 以 \n 结尾
    firstCardJson = firstCardJson.substr(0, firstCardJson.size() - 1);
    int remainCardNum = (int) (gameTable.getDealer().getCardStack().size()); // 剩余牌数 默认：108 - 2 * 8 - 1

    // 发送消息
    char *msg = new char[BUFSIZ]; // 该消息可能较长
    sprintf(msg, "uno02 gamestart %d %s %s\r\n", remainCardNum, firstCardJson.c_str(), playersJson.c_str());

    multicast(players, msg);
    delete[] msg;
}


void GameService::drawCard(string username, string roomNum) {
    int room = stoi(roomNum);
    GameTable &gameTable = gameTables[room];
    const vector<Player *> &players = gameTable.getPlayers();

    if (gameTable.getStatus() != GameTable::GAMING)
        return;
    // 修改模型层；稍后将牌返回给房间内所有客户
    UNOCard unoCard = gameTable.drawCard(username);
    string nextPlayerName = gameTable.nextTurn(username); // 调整出牌顺序

    // 返回 drawcard
    char *msg = new char[1024];
    string unoCardJson = unoCard.toJson(); // 以 \n 结尾
    unoCardJson = unoCardJson.substr(0, unoCardJson.size() - 1);
    sprintf(msg, "uno02 drawcard %s %s\r\n", username.c_str(), unoCardJson.c_str());
    multicast(players, msg);
    memset(msg, 0, sizeof(msg));
    // 返回 nextTurn
    sprintf(msg, "uno02 turn %s\r\n", nextPlayerName.c_str());
    multicast(players, msg);
    delete[]msg;

    remainCard(room); // 更新剩余卡牌数
}

void GameService::remainCard(int room) {
    GameTable &gameTable = gameTables[room];
    const vector<Player *> &players = gameTable.getPlayers();

    char *msg = new char[64];
    int remainCardNum = static_cast<int>(gameTable.getDealer().getCardStack().size());
    sprintf(msg, "uno02 remaincard %d\r\n", remainCardNum);
    multicast(players, msg);
    delete[]msg;
}

void GameService::playCard(const vector<string> &request) {
    string username = request[2];
    string roomNum = request[3];
    string cardNumStr = request[4];
    int wildColor = -1;

    int cardNum = stoi(cardNumStr);
    int room = stoi(roomNum);
    GameTable &gameTable = gameTables[room];
    auto players = gameTable.getPlayers();

    // 删掉玩家手中的牌
    Player *player = gameTable.getPlayerByUsername(username);
    UNOCard topCard = player->playCard(cardNum); // 删除打出的牌，并计数
    string nextPlayerName = gameTable.nextTurn(username); // 调整出牌顺序

    if (topCard.getType() == UNOCard::ACTION) { // 使用动作牌
        gameTable.nextTurn(nextPlayerName);
        if (topCard.getValue() == UNOCard::REVERSE || topCard.getValue() == UNOCard::SKIP) {
            // 使用 SKIP 牌
        } else if (topCard.getValue() == UNOCard::DRAW2PLUS) {
            // 使用 DRAW TWO 牌
            gameTable.drawCards(nextPlayerName, 2);
        }
    } else if (topCard.getType() == UNOCard::WILD) { // 使用万能牌
        wildColor = stoi(request[5]);
        if (topCard.getValue() == UNOCard::DRAW4PLUS) {
            // 使用 DRAW FOUR 牌
            gameTable.drawCards(nextPlayerName, 4);
        }
    }

    string topCardJson = topCard.toJson();

    // 判断是否说 UNO
    int playerCardNum = static_cast<int>(player->getMyCards().size());
    if (playerCardNum == 1 && !player->isSaidUNO()) { // 忘记说 UNO
        string error = player->getUsername() + " 忘了说 UNO";
        setError(error, room); // todo 告知房间内所有玩家，该玩家忘记说 UNO
        gameTable.drawCards(username, 2); // 为该用户加 2 张牌
    } else if (playerCardNum > 2) { // 在大于 2 张牌时说 UNO 无效，需要在剩余恰好 2 张牌时说
        player->setSaidUNO(false);
    }

    // 客户端需更新牌桌上的牌以及手中的牌
    char *msg = new char[BUFSIZ];
    sprintf(msg, "uno02 playcard %s %s %s\r\n", username.c_str(), topCardJson.c_str(),
            getPlayersJson(gameTable).c_str());
    multicast(players, msg);
    delete[] msg;

    remainCard(room); // 更新剩余卡牌数
    if (topCard.getType() == UNOCard::WILD)
        setWildColor(room, wildColor); // 万能牌指定颜色
    checkGameOver(room); // 检查房间内游戏是否已结束
}

string GameService::getPlayersJson(GameTable &gameTable) {
    const vector<Player *> &players = gameTable.getPlayers();
    string ret;
    for (Player *const roomPlayer:players) {
        string playerJson = roomPlayer->toJson(); // 以 \n 结尾
        playerJson = playerJson.substr(0, playerJson.size() - 1);
        ret += playerJson + " "; // 玩家之间以空格分隔
    }
    ret = ret.substr(0, ret.size() - 1); // 删除最后的空格
    return ret;
}

void GameService::setError(string error, int room) { // uno02 seterror error
    GameTable &gameTable = gameTables[room];
    const auto &players = gameTable.getPlayers();
    char *msg = new char[64];
    sprintf(msg, "uno02 seterror %s\r\n", error.c_str());
    multicast(players, msg);
    delete[]msg;
}

/**
 * 检查房间内游戏是否已结束
 * @param room 房间号
 * @return 结束返回 true；游戏中返回 false
 */
bool GameService::checkGameOver(int room) {
    bool ret = false;
    GameTable &gameTable = gameTables[room];
    const auto &players = gameTable.getPlayers();
    for (Player *player:players) {
        if (player->getMyCards().empty())
            ret = true;
    }
    if (ret) {
        for (Player *player:players) { // 所有玩家不得再出牌
            player->setIsMyTurn(false);
        }
        char *msg = new char[32];
        sprintf(msg, "uno02 gameover\r\n");
        multicast(players, msg);
        delete[]msg;
    }
    return ret;
}

void GameService::setWildColor(int room, int color) {
    GameTable &gameTable = gameTables[room];
    const auto &players = gameTable.getPlayers();
    char *msg = new char[32];
    sprintf(msg, "uno02 wildcolor %d\r\n", color);
    multicast(players, msg);
    delete[]msg;
}

void GameService::sayUNO(string username, string roomNum) {
    int room = stoi(roomNum);
    GameTable &gameTable = gameTables[room];
    Player *player = gameTable.getPlayerByUsername(username);
    player->setSaidUNO(true);
}
