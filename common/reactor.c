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

void add_event_ptr(int epollfd, int fd, int events, struct User *user) {
    struct epoll_event ev;
    ev.events = events;
    ev.data.ptr = (void *)user;
    // 向从反应堆(subfd) 中注册套接字为 fd 的事件
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);
}

int add_to_reactor(int fd, struct User *user) {
    int sub = user->fd;
    users[sub] = *user;

    add_event_ptr(fd, users[sub].fd, EPOLLIN | EPOLLET, &users[sub]);
}

void del_from_reactor(int fd) {
    epoll_ctl(subfd, EPOLL_CTL_DEL, fd, NULL);
}

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
            
            // 这里用于测试第一次发给别人后，别人回复给我的消息是否接受到
            /*
            if((msg.type & CHAT_ACK) && (msg.type & CHAT_SYN)) {
                printf("CCCCCCCCC\n");
            }
            */
            
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
}
