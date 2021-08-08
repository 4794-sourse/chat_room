/*************************************************************************
	> File Name: ../common/common.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 10:33:04 AM CST
 ************************************************************************/

#ifndef _COMMON_H
#define _COMMON_H
int make_nonblock(int fd);
int make_block(int fd);
char *get_value(const char *path, const char *key);
#endif
