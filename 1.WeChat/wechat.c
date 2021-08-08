/*************************************************************************
	> File Name: wechat.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 10:24:31 AM CST
 ************************************************************************/

#include "head.h"
#define MAX 10

char *config = "./chat.conf";
int port = 0;
char name[20] = {0}, msg[512] = {0};
struct User *users;
int subfd;

void logout(int signum) {
    struct Msg l_msg;
    l_msg.type = CHAT_FIN;
    strcpy(l_msg.from, name);
    strcpy(l_msg.msg, "Bye!");
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
    pthread_t c_discover, heart_beat_tid, reactor_tid, send_tid;
    // -p:端口号   -n:姓名   -m:消息
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

    // get_value(config, "name");
    // return 0;

    // SIGINT 信号代表由 InterruptKey 产生，通常是 CTRL+C 或者是 DELETE
    // 中断程序运行时会按下 CTRL+C，这时 signal 会调用 logout 函数
    signal(SIGINT, logout);

    // get_value(common.c)
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

    // epoll_create(int size)创建一个新的epoll实例
    // 从Linux 2.6.8开始，size参数将被忽略，但必须大于零
    // epoll_create() 返回引用新 epoll 实例的文件描述符。 该文件描述符用于随后的所有对 epoll 接口的调用。
    // 当不再需要时，应使用 close(2) 关闭epoll_create() 返回的文件描述符。 关闭所有引用 epoll 实例的文件描述符后，内核将销毁该实例并释放关联的资源以供重用。
    // 成功时，这些系统调用将返回非负文件描述符。 
    // 发生错误时，将返回 -1，并且将 errno 设置为指示错误。 
    // epollfd: 主反应堆 
    if((epollfd = epoll_create(1)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    // subfd: 从反应堆
    if((subfd = epoll_create(1)) < 0) {
        perror("epoll_create");
        exit(1);
    }

    /*
    typedef union epoll_data {
        void        *ptr;
        int         fd;
        __uint32_t  u32;
        __uint64_t  u64;
    } epoll_data_t;   // 保存触发事件的某个文件描述符相关的数据

    struct epoll_event {
        __uint32_t      events;      // epoll event
        epoll_data_t    data;        // User data variable
    };

    其中 events 表示感兴趣的事件和被触发的事件，可能的取值为：
    EPOLLIN：表示对应的文件描述符可以读；
    EPOLLOUT：表示对应的文件描述符可以写；
    EPOLLPRI：表示对应的文件描述符有紧急的数可读；

    EPOLLERR：表示对应的文件描述符发生错误；
    EPOLLHUP：表示对应的文件描述符被挂断；
    EPOLLET：ET 的 epoll 工作模式；
    */

    struct epoll_event ev, events[MAX];
    users = (struct User *)calloc(MAXUSER, sizeof(struct User));

    ev.events = EPOLLIN;
    ev.data.fd = listener;

    /*
    epoll_ctl:
    函数原型: int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
    功能：用于控制某个文件描述符上的事件，可以注册事件，修改事件，删除事件
    epfd：由 epoll_create 生成的 epoll 专用的文件描述符
    op：要进行的操作，EPOLL_CTL_ADD 注册、EPOLL_CTL_MOD 修改、EPOLL_CTL_DEL 删除事件
    fd：关联的文件描述符
    event：指向 epoll_event 的指针
    返回值：成功：0   失败：-1
    */
    // 把 listener 描述符注册到 epollfd，注册的事件是 ev
    if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listener, &ev) < 0) {
        perror("epoll_ctl");
        exit(1);
    }

    // 创建线程
    // c_discover：给 .iplist 中所有的 ip 发送 SYN 包
    pthread_create(&c_discover, NULL, client_discover, NULL);
    pthread_create(&heart_beat_tid, NULL, heart_beat, NULL);
    pthread_create(&reactor_tid, NULL, reactor, NULL);
    pthread_create(&send_tid, NULL, send_chat, NULL);

    while(1) {
        /*
        epoll_wait:
        函数原型：int epoll_wait(int epfd, struct epoll_event * events, int maxevents, int timeout)
        功能：该函数用于轮询 I/O 事件的发生
        epfd：由 epoll_create 生成的 epoll 专用的文件描述符
        epoll_event：用于回传代处理事件的数组
        maxevents：每次能处理的事件数
        timeout：等待 I/O 事件发生的超时值,当 timeout 为 -1 时代表一直等到事件发生或出错为止,且返回值不可能为 0
        返回值：成功：返回发生的事件数   失败：-1
        */
        // epoll_wait 将返回事件存储到 events
        int nfds = epoll_wait(epollfd, events, MAX, -1);
        DBG("⚙ "RED"<Main Reactor>"NONE" : Got Events...\n");

        if(nfds < 0) {
            perror("epoll_wait");
            exit(1);
        }

        for(int i = 0; i < nfds; i++) {
            struct User newuser;

            // udp_accept(common/udp_server.c)
            if((new_fd = udp_accept(listener, &newuser)) < 0) {
                fprintf(stderr, "error in udp_accept!\n");
                continue;
            }
            // listener 是一个前置反应堆(主反应堆)，它负责用户的连接请求
            // 添加到从反应堆(后置反应堆)
            // 如果 listener 判断一个用户可以连接，然后就把这个用户注册到这个从反应堆中，之后这个用户的的所有信息都会直接被从反应堆去处理
            add_to_reactor(subfd, &newuser);
            DBG("⚙ "RED"<Main Reactor>"NONE" : Adding client to sub reactor...\n");
        }
    }

    return 0;
}
