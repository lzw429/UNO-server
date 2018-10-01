# 应用层协议

每条消息以 "\r\n" 结束。

## 01. 用户状态

|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|用户登录   |uno01 login username|
|服务器|客户端|用户登录成功|uno01 login username 1|
|服务器|客户端|用户登录失败|uno01 login username 0|
|客户端|服务器|用户离线   |uno01 logout username|

## 02. 游戏操作

### 1. 游戏大厅
|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|请求大厅数据|uno02 hall|
|服务器|客户端|返回大厅数据|uno02 hall\r\n\r\nContent|

### 2. 对局初始化

|发送方|接受方|含义|内容|
|---|---|---|---|
|客户端|服务器|请求对局信息|uno02 gamestatus username|
|客户端|服务器|打牌|uno02 playcard username|
|客户端|服务器|抽牌|uno02 drawcard username|
|客户端|服务器|说 UNO|uno02 sayuno username|
