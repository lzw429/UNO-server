# 参数

[UNOServer](https://github.com/lzw429/UNO-server) 
和 [UNOClient](https://github.com/lzw429/UNO-Client-2) 
统一使用的参数。

## 1. 房间状态

|状态|常量|值|
|---|---|---|
|空闲|`IDLE`|`0`|
|等待|`WAITING`|`1`|
|游戏中|`GAMING`|`2`|

## 2. 游戏模式
|模式|常量|值|
|---|---|---|
|联机|`ONLINE`|`2`|

## 3. 卡牌

|颜色|常量|值|
|---|---|---|
|红色|`RED`|`0`|
|蓝色|`BLUE`|`1`|
|绿色|`GREEN`|`2`|
|黄色|`YELLOW`|`3`|
|黑色|`BLACK`|`4`|

|类别|常量|值|
|---|---|---|
|数字卡|`NUMBER`|`1`|
|动作卡|`ACTION`|`2`|
|万能卡|`WILD`|`3`|

## 4. 消息
|消息|相关请求或响应|值|
|---|---|---|
|忘记说 UNO|`uno02 seterror `|`0 username`|