//
// Created by syh on 10/15/18.
//
/*
* Reference: https://www.geeksforgeeks.org/implementation-diffie-hellman-algorithm/
*/
#ifndef UNOSERVER_INFOSECURITY_H
#define UNOSERVER_INFOSECURITY_H

#include <cmath>
#include <bits/stdc++.h>

using namespace std;

/**
* 带模运算的指数函数
* @param a
* @param b
* @param P
* @return 返回 a^b mod P
*/
long long int power(long long int a, long long int b, long long int P);

/**
* 生成随机数
* @param limit 随机数上限
* @return long long int 范围内的随机数
*/
long long int randomNumber(long long limit = LONG_LONG_MAX);

/**
* 生成随机数
* @param limit 随机数上限
* @return int 范围内的随机数
*/
int randomNumber(int limit = INT_MAX);

#endif //UNOSERVER_INFOSECURITY_H