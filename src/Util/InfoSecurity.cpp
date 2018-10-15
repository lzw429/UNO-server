//
// Created by syh on 10/15/18.
//
#include "InfoSecurity.h"

long long int power(long long int a, long long int b, long long int P) {
    if (b == 1)return a;
    return ((long long int) pow(a, b) % P);
}

long long int randomNumber() {
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(1, LONG_LONG_MAX);
    return (long long) (dis(gen));
}

