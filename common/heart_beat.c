/*************************************************************************
	> File Name: heart_beat.c
	> Author: 
	> Mail: 
	> Created Time: Fri 07 May 2021 06:11:14 PM CST
 ************************************************************************/

#include "head.h"

extern struct User *users;
extern char name[20];
//发送心跳线程函数，检测在线用户
void *heart_beat(void *arg) {
    struct Msg heart;
    heart.type = CHAT_HEART;
    strcpy(heart.from, name);
    while(1) {
        for(int i = 0; i < MAXUSER; i++) {
            if(users[i].flag) {
                if(users[i].cnt <= 0) {
                    DBG("🔺 "RED"<%s>"NONE"is removed by heart breat...\n", users[i].name);
                    users[i].flag = 0;
                    del_from_reactor(users[i].fd);
                    DBG("🔧 "RED"<%s>"NONE"is removed from sub reactor...\n", users[i].name);
                    close(users[i].fd);
                }
                send(users[i].fd, (void *)&heart, sizeof(heart), 0);
                users[i].cnt--;
            }
        }
        sleep(5);
    }
}
