/*************************************************************************
	> File Name: send_chat.c
	> Author: 
	> Mail: 
	> Created Time: Fri 07 May 2021 09:31:51 PM CST
 ************************************************************************/

#include "head.h"

extern char name[20];
extern struct User *users;

void *send_chat(void *arg) {
    struct Msg msg;
    strcpy(msg.from, name);
    msg.type = CHAT_MSG;
    while(1) {
        memset(msg.msg, 0, sizeof(msg.msg));
        printf(L_PINK"Please Input:\n"NONE);
        scanf("%s", msg.msg);
        for(int i = 0; i < MAXUSER; i++) {
            if(users[i].flag) {
                send(users[i].fd, &msg, sizeof(msg), 0);
            }
        }
    }
    return NULL;
}
