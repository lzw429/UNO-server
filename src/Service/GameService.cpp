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
            Player *player0 = gameTable.getPlayer(0);
            Player *player1 = gameTable.getPlayer(1);
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
        printf("GameService: this user does not exist");
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
    } else { // 房间继续等待其他玩家
        gameTable.setStatus(GameTable::WAITING);
    }
    printTime();
    printf("GameService: player %s has quit room #%d\n", username.c_str(), room);
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
    gameTable.gameStart();
    // 发送消息
    const vector<Player *> &players = gameTable.getPlayers();
    for (Player *const player:players) {
        string playerJson = player->toJson(); // 以 \n 结尾
        playerJson = playerJson.substr(0, playerJson.size() - 1);
        string msg = "uno02 gamestart " + playerJson + "\r\n";
        unicast(player->getFd(), msg.c_str());
    }
}
