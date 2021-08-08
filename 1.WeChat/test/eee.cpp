/*************************************************************************
	> File Name: eee.cpp
	> Author: 
	> Mail: 
	> Created Time: Sat 01 May 2021 04:37:41 PM CST
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>

using namespace std;

int a[100];

int main() {
    int n, t = 0;
    cin >> n;

    while(n) {
        a[++t] = n & 1;
        n >>= 1;
    }
    
    int p = 0;
    for(int i = t; i > 0; i--) {
        cout << a[i];
        if(++p % 8 == 0) cout << ' ';
    }
    cout << endl;

    return 0;
}
