//
// Created by syh on 10/7/18.
//

#ifndef UNOSERVER_TIMEUTIL_H
#define UNOSERVER_TIMEUTIL_H

#include <ctime>
#include <chrono>
#include <tiff.h>
#include <cstdio>
#include <cstring>
#include <string>

using namespace std;
using namespace chrono;

time_t getTimeStamp();

string getTimeInMillis();

void printTime();

#endif //UNOSERVER_TIMEUTIL_H
