# 应用层协议

每条消息以`\r\n`结束，消息间以`\f`分隔。

## 01. 用户状态

|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|用户登录   |uno01 login username|
|服务器|客户端|用户登录成功|uno01 login username 1|
|服务器|客户端|用户登录失败|uno01 login username 0|
|客户端|服务器|用户离线   |uno01 logout username|

## 02. 游戏大厅
|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|请求大厅数据|uno02 hall|
|服务器|客户端|返回大厅数据|uno02 hall\r\n\r\nContent|

Content 每行代表一个房间: player1,player2,status\r\n

## 03. 房间

|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|进入房间|uno02 enterroom username roomnumber|
|服务器|客户端|进入房间成功|uno02 enterroom roomnumber 1|
|服务器|客户端|进入房间失败|uno02 enterroom roomnumber 0|
|服务器|客户端|广播房间状态|uno02 roomstatus roomnumber player1,player2,status|
|客户端|服务器|退出房间|uno02 quitroom username|

## 04. 对局信息

|发送方|接受方|含义|内容|
|---|---|---|---|
|服务器|客户端|初始化对局信息|uno02 gamestart remainCardNum topCardJson playerJson|
|服务器|客户端|||

## 05. 游戏操作

|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|打牌|uno02 playcard username|
|客户端|服务器|抽牌|uno02 drawcard username roomnum|
|客户端|服务器|说 UNO|uno02 sayuno username roomnum|
|服务器|客户端|出牌轮次返回|uno02 turn username|
|服务器|客户端|抽牌返回|uno02 drawcard username unoCardJson|
