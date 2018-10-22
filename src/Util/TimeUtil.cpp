//
// Created by syh on 10/9/18.
//

#include "TimeUtil.h"

time_t getTimeStamp() {
    time_point<system_clock, milliseconds> tp = time_point_cast<milliseconds>(
            system_clock::now());
    auto tmp = duration_cast<milliseconds>(tp.time_since_epoch());
    time_t timestamp = tmp.count();
    return timestamp;
}

string getTimeInMillis() {
    auto timestamp = getTimeStamp();
    char *timeInMillis = new char[32];
    int64 milli = timestamp + (int64) 8 * 60 * 60 * 1000;
    // 此处转化为东八区北京时间，如果是其它时区需要按需求修改
    auto mTime = milliseconds(milli);
    auto tp = time_point<system_clock, milliseconds>(mTime);
    auto tt = system_clock::to_time_t(tp);
    tm *now = gmtime(&tt);

    sprintf(timeInMillis, "%4d-%02d-%02d %02d:%02d:%02d:%02d", now->tm_year + 1900,
            now->tm_mon + 1, now->tm_mday, now->tm_hour,
            now->tm_min, now->tm_sec, (int) (milli % 1000));
    string res = timeInMillis;
    delete[] timeInMillis;
    return res;
}

int getRandomIntByTime() {
    auto timestamp = getTimeStamp();
    int64 milli = timestamp + (int64) 8 * 60 * 60 * 1000;
    // 此处转化为东八区北京时间，如果是其它时区需要按需求修改
    int ret = (int) milli % 0x3f3f3f3f;
    if (ret < 0)
        ret = -ret;
    return ret;
}

/**
 * 标准输出当前时间，精确到毫秒
 */
void printTime() {
    printf("[%s] ", getTimeInMillis().c_str());
}