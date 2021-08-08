/*************************************************************************
	> File Name: reactor.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 02:44:11 PM CST
 ************************************************************************/

#ifndef _REACTOR_H
#define _REACTOR_H
int add_to_reactor(int fd, struct User *user);
void *reactor(void *arg);
void del_from_reactor(int fd);
#endif
