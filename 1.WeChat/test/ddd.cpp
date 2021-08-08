/*************************************************************************
	> File Name: ddd.cpp
	> Author: 
	> Mail: 
	> Created Time: Thu 29 Apr 2021 04:03:41 PM CST
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include "../common/head.h"

using namespace std;

int main() {
    FILE *fp;
    fp = fopen("file", "r");
    char p[30] = {0};
    fscanf(fp, "%s", p);
    cout << p << endl;
    fscanf(fp, "%s", p);
    cout << p << endl;
    return 0;
}
