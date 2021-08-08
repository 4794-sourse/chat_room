/*************************************************************************
	> File Name: client_discover.c
	> Author: 
	> Mail: 
	> Created Time: Tue 20 Apr 2021 07:44:25 AM CST
 ************************************************************************/

#include "head.h"

extern char *config;
extern char name[20];
extern char msg[512];

// 开辟一个线程来处理
void *client_discover(void *arg) {
    char file[512] = {0};
    FILE *fp;
    struct Msg request;
    struct sockaddr_in client;
    int sockfd;
    if((sockfd = socket_udp()) < 0) {
        perror("socket_udp");
        exit(1);
    }

    strcpy(request.from, name);
    strcpy(request.msg, msg);
    request.type = CHAT_SYN;
    strcpy(file, get_value(config, "iplist"));
    if((fp = fopen(file, "r")) == NULL) {
        perror("fopen");
        exit(1);
    }

    DBG("<Ip Discover> start...\n");
    client.sin_family = AF_INET;
    client.sin_port = htons(8888);

    while(!feof(fp)) {
        char ip[20] = {0};
        fscanf(fp, "%s", ip);
        if(!strlen(ip)) continue;
        client.sin_addr.s_addr = inet_addr(ip);
        sendto(sockfd, (void *)&request, sizeof(request), 0, (struct sockaddr *)&client, sizeof(client));

        //DBG(GREEN"<Discover Send>"NONE" I'm %s, msg is %s...\n", request.from, request.msg);
        //DBG(GREEN"<Discover Send>"NONE" I'm %s, msg is %s...\n", name, msg);
    }
    DBG("<Ip Discover> end...\n");
}
