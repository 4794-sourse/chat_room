/*************************************************************************
	> File Name: ccc.cpp
	> Author: 
	> Mail: 
	> Created Time: Wed 28 Apr 2021 07:46:17 PM CST
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "../common/head.h"

using namespace std;

int main() {
    // char s[20] = "8.130.49.164";
    char s[20] = "47.94.157.77";
    long long a = inet_addr(s);
    cout << a << endl;
    int t = 0;
    while(a) {
        int x = a % 2;
        cout << x;
        a >>= 1;
        if(++t == 8) {
            cout << ' ';
            t = 0;
        }
    }
    cout << endl;

    struct in_addr p;
    inet_aton(s, &p);
    cout << p.s_addr << endl;

    char *m = inet_ntoa(p);
    cout << m << endl;
    return 0;
}
