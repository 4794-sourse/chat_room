/*************************************************************************
	> File Name: udp_server.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 09:51:20 AM CST
 ************************************************************************/

#include "head.h"

int socket_create_udp(int port) {
    int listener;

    // 函数原型: int socket(int domain, int type, int protocol);
    // domain 参数指定一个通信域,这将选择将用于通信的协议系列. 这些系列在 <sys/socket.h> 中定义
    // type 可以指定通信语义
    // socket() 创建一个用于通信的端点，并返回引用该端点的文件描述符。 
    // 成功调用返回的文件描述符将是当前未为该进程打开的编号最小的文件描述符。 
    // 成功后，将返回新套接字的文件描述符。如果出错，则返回-1，并正确设置 errno
    // AF_INET: IPv4 网络协议
    // SOCK_DGRAM: 支持数据报（固定长度的无连接，不可靠消息）
    if((listener = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    /*
    struct sockaddr_in {
        sa_family_t     sin_family;         // 地址族(Address Family)
        uint16_t        sin_port;           // 16 位 TCP/UDP 端口号
        struct in_addr  sin_addr;           // 32 位 IP 地址
        char            sin_zero[8]         // 不使用
    };

    strcut in_addr {
        In_addr_t       s_addr;             // 32 位 IPv4 地址
    };

    htonl() 函数将无符号整数 hostlong 从主机字节顺序转换为网络字节顺序
    htons() 函数将无符号的短整数 hostshort 从主机字节顺序转换为网络字节顺序
    INADDR_ANY: 就是指定地址为0.0.0.0的地址
    */
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);
    unsigned long opt = 1;

    // setsockopt() 函数用于任意类型、任意状态套接口的设置选项值。
    // 函数原型: int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // bind: 给套接字绑定一个名称
    // 函数原型: int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    // 使用 socket 创建套接字时，该套接字存在于名称空间（地址族）中，但未分配地址。
    // bind() 将 addr 指定的地址分配给文件描述符 sockfd 所引用的套接字。
    // addrlen 指定 addr 指向的地址结构的大小（以字节为单位）
    // 通常，在 SOCK_STREAM 套接字接收连接之前，必须使用 bind() 分配本地地址
    // 成功时返回0，失败时返回-1
    /*
    struct sockaddr {
        sa_family_t     sa_family;
        char            sa_data[14];
    };
    */
    if(bind(listener, (struct sockaddr *)&server, sizeof(server)) < 0) {
        return -1;
    }

    return listener;
}

int udp_accept(int fd, struct User *user) {
    int new_fd, ret;
    struct sockaddr_in client;
    struct Msg request, response;
    socklen_t len = sizeof(client);
    bzero(&request, sizeof(request));
    bzero(&response, sizeof(response));

    // printf("client.sin_port 1: %d\n", client.sin_port);
    // printf("client.sin_addr.s_addr 1: %d\n", client.sin_addr.s_addr);

    // fd 在 wechat.c 中是 listener，listener 起到监听的作用
    // 当对方通过 sendto 发送信息后，wechat.c 中的 epoll_wait 会返回收到的事件数量
    ret = recvfrom(fd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, &len);

    // printf("client.sin_port 2: %d\n", client.sin_port);
    // printf("client.sin_addr.s_addr 2: %d\n", client.sin_addr.s_addr);

    if(ret != sizeof(request)) {
        response.type = CHAT_FIN;
        strcpy(response.msg, "Login Request Error!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }


    /*
    if(check_online(&request)) {
        response.type = CHAT_FIN;
        strcpy(response.msg, "Already login!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }
    */

    //printf("request.type : %d\n", request.type);
    if(request.type & CHAT_FIN) {
        return -1;
    } else if((request.type & CHAT_SYN) && (request.type & CHAT_ACK)) {
        DBG(GREEN"<Acceptor>"NONE" : SYNACK recieved!\n");
    } else if(request.type != CHAT_SYN) {
        response.type = CHAT_FIN;
        strcpy(response.msg, "Error Request!");
        sendto(fd, (void *)&response, sizeof(response), 0, (struct sockaddr *)&client, len);
        return -1;
    }

    //printf("---Test:%s\n", request.from);
    printf(GREEN"<Acceptor>"NONE" : "YELLOW"[%s]"NONE" %s\n", request.from, request.msg);

    // 收到对方消息后，回复对方，用 response 存储信息回复
    strcpy(user->name, request.from);
    user->flag = FL_ONLINE;
    user->cnt = 5;    // 5次机会，为0说明超时

    // udp_connect 进行连接操作，连接成功后使用 send 就可以发送信息，无需使用sendto
    new_fd = udp_connect(&client);   
    if(new_fd < 0) {
        perror("udp_connect");
        return -1;
    }
    user->fd = new_fd;
    char buff[512] = {0};
    char buff2[512] = {0};
    sprintf(buff, "Login success!");
    sprintf(buff2, "%s", user->name);
    strcpy(response.from, buff2);
    strcpy(response.msg, buff);
    response.type = CHAT_ACK | CHAT_SYN;
    send(new_fd, (void *)&response, sizeof(response), 0);
    
    return new_fd; 
}

