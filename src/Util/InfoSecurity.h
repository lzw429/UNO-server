//
// Created by syh on 10/15/18.
//

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
 * @return long long int 范围内的随机数
 */
long long int randomNumber();

#endif //UNOSERVER_INFOSECURITY_H
