/*************************************************************************
	> File Name: fff.cpp
	> Author: 
	> Mail: 
	> Created Time: Sat 01 May 2021 04:59:19 PM CST
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "../common/head.h"

using namespace std;

void func(char *ip) {

    int p = inet_addr(ip);
    cout << "inet_addr(" << ip << "): " << p << endl;


    struct in_addr m;
    inet_aton(ip, &m);
    cout << "inet_ntoa(" << p << "): " << inet_ntoa(m) << endl;

    cout << endl;

    return ;
}

void func2() {
    struct in_addr m;
    int x = 1919331038;
    m.s_addr = x;
    cout << "inet_ntoa(" << x << "): " << inet_ntoa(m) << endl;
    cout << endl;
    return ;
}

void func3() {
    cout << 1<< endl;
    for(int i = 0; i <= 99999; i++) {
        if(htons(i) == 12979) {
            cout << i << endl;
            return ;
        }
    }
    cout << -1 << endl;
    return ;
}

int main() {

    func("8.130.49.164");
    //func("47.94.157.77");
    //func("169.254.182.183");

    //func2();
    func3();
    return 0;
}
