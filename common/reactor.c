/*************************************************************************
	> File Name: reactor.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 02:41:36 PM CST
 ************************************************************************/

#include "head.h"

extern int subfd;
extern struct User *users;
extern char name[20];

// 向从反应堆(subfd) 中注册套接字为 fd 的事件
void add_event_ptr(int subfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *)user;
    epoll_ctl(subfd, EPOLL_CTL_ADD, fd, &ev);
    return ;
}

//添加从反应堆中
int add_to_reactor(int subfd, struct User *user) {
    int sub = user->fd;
    users[sub] = *user;
    add_event_ptr(subfd, users[sub].fd, EPOLLIN | EPOLLET, &users[sub]);
    return 0;
}

//在从反应堆中删除
void del_from_reactor(int fd) {
    epoll_ctl(subfd, EPOLL_CTL_DEL, fd, NULL);
}

//线程函数，处理通讯消息类型
void *reactor(void *arg) {
    struct epoll_event events[20];
    struct Msg msg;
    while(1) {
        int nfds = epoll_wait(subfd, events, 20, -1);
        // 在线程中调用 exit(1) 会让整个程序全部退出
        if(nfds < 0) exit(1);
        for(int i = 0; i < nfds; i++) {
            int fd = ((struct User *)(events[i].data.ptr))->fd;
            recv(fd, &msg, sizeof(msg), 0);
            if(strlen(msg.from) > 0) strcpy(users[fd].name, msg.from);
            users[fd].cnt = 5;
            if(msg.type & CHAT_HEART) {
                DBG(YELLOW"<%s>"NONE" 🧡  -> ️"L_BLUE"<Me>"NONE"\n", users[fd].name);
                msg.type = CHAT_ACK;
                strcpy(msg.from, name);
                send(fd, &msg, sizeof(msg), 0);
            } else if(msg.type & CHAT_ACK) {
                DBG(L_BLUE"<Me>"NONE" 🙍 ‍️💖 ‍🙍 -> "YELLOW"<%s>"NONE" 💖 \n", users[fd].name);
            } else if(msg.type & CHAT_FIN) {
                DBG("🔺 "BLUE" <%s>"NONE" 拜拜~ 🖐  \n", users[fd].name);
                // epoll_ctl delete from sub reactor.
                del_from_reactor(fd);
                close(fd);
                users[fd].flag = 0;
                printf("🔺 "RED" <%s>"NONE" is leaving!\n", msg.from);
                DBG("🔧 "RED" <%s>"NONE" is remove from subreactor...\n", msg.from);
            } else if(msg.type & CHAT_MSG) {
                printf("🔵 "YELLOW" <%s>"NONE"：%s\n", msg.from, msg.msg);
            }
            else {
                printf("🌼 \n");
            }
        }
    }
    return NULL;
}
