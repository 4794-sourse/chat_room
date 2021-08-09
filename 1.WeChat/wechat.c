/*************************************************************************
	> File Name: wechat.c
	> Author: captand
	> Mail: 
	> Created Time: Sun 08 Aug 2021 04:53:22 PM CST
 ************************************************************************/

#include<stdio.h>
#include "head.h"
#define MAX 10

char *config = "./chat.conf";          //配置文件
int port = 0;                          //端口信息
char name[20] = {0}, msg[512] = {0};   //保存配置文件中的名称与信息
struct User *users;                    //保存聊天成员相关信息
int subfd;                             //从反应堆的epoll实例

//退出显示信息
void logout(int signum) {
    struct Msg l_msg;
    l_msg.type = CHAT_FIN;
    strcpy(l_msg.from, name);
    strcpy(l_msg.msg, "Bye!");
    //给所有在线的聊天成员发送自己的退出信息
    for(int i = 0; i < MAXUSER; i++) {
        if(users[i].flag & FL_ONLINE) {
            send(users[i].fd, &l_msg, sizeof(l_msg), 0);
        }
    }
    printf("\n 🚫  "RED"Logout from wechat system..."NONE"\n");
    exit(0);
}

int main(int argc, char **argv) {
    int opt, listener, epollfd, new_fd;
    pthread_t c_discover, heart_beat_tid, reactor_tid, send_tid; //设置四个线程
    //命令行解析   -p:端口号   -n:姓名   -m:消息
    while((opt = getopt(argc, argv, "p:n:m:")) != -1) {
        switch(opt) {
            case 'p':
                port = atoi(optarg);
                break;
            case 'm':
                strcpy(msg, optarg);
                break;
            case 'n':
                strcpy(name, optarg);
                break;
            default:
                fprintf(stderr, "Usage : %s -n name -p port -m msg\n", argv[0]);
                exit(1);
        }
    }

    //捕捉函数： SIGINT 信号代表由 InterruptKey 产生，通常是 CTRL+C 或者是 DELETE
    //中断程序运行时会按下 CTRL+C，这时 signal 会调用 logout 函数，进行退出操作
    signal(SIGINT, logout);

    //在未有命令行参数时，在配置文件chat.conf获取相关信息——名称、信息、端口
    if(!strlen(name)) strncpy(name, get_value(config, "name"), 20);
    if(!strlen(msg)) strncpy(msg, get_value(config, "msg"), sizeof(msg));
    if(!port) port = atoi(get_value(config, "port"));
    
    DBG(BLUE"<Starting...>"NONE"name = %s, port = %d, msg = %s\n", name, port, msg);

    // socket_create_udp(udp_server.c)
    if((listener = socket_create_udp(port)) < 0) {
        perror("socket_create_udp");
        exit(1);
    }

    DBG(BLUE"<Starting...>"NONE"listener %d ok!\n", listener);

    //创建主反应堆的epoll实例——epollfd
    if((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    //创建从反应堆的epoll实例——subfd
    if((subfd = epoll_create(1)) < 0) {
        perror("epoll_create");
        exit(1);
    }
    
    //创建epoll事件，和保存事件的结构体数组events
    struct epoll_event ev, events[MAX];
    users = (struct User *)calloc(MAXUSER, sizeof(struct User));

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    // 把 listener 描述符注册到 epollfd，注册的事件是 ev
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    // 创建四个线程线程
    // c_discover：给 .iplist 中所有的 ip 发送 SYN 包
    // heart_beat_tid: 给所有聊天成员发送心跳
    // reactor_tid: 处理相关聊天成员信息、对接收包的不同属性做出处理
    // send_tid: 处理发送消息相关操作
    pthread_create(&c_discover, NULL, client_discover, NULL);
    pthread_create(&heart_beat_tid, NULL, heart_beat, NULL);
    pthread_create(&reactor_tid, NULL, reactor, NULL);
    pthread_create(&send_tid, NULL, send_chat, NULL);

    while(true) {
        int nfds = epoll_wait(epollfd, events, MAX, -1);
        DBG("⚙ "RED"<Main Reactor>"NONE" : Got Events...\n");
        if(nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }

        for(int i = 0; i < nfds; i++) {
            struct User newuser;
            if((new_fd = udp_accept(listener, &newuser)) < 0) {
                fprintf(stderr, "error in udp_accept!\n");
                continue;
            }
            // 添加到从反应堆(后置反应堆)
            add_to_reactor(subfd, &newuser);
            DBG("⚙ "RED"<Main Reactor>"NONE" : Adding client to sub reactor...\n");
        }
    }
    return 0;
}
