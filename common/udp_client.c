/*************************************************************************
	> File Name: udp_client.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 10:15:25 AM CST
 ************************************************************************/

#include "head.h"

extern int port;

//创建udp的socket的描述符
int socket_udp() {
    int sockfd;

    if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        return -1;
    }

    return sockfd;
}

//建立udp的connect连接
int udp_connect(struct sockaddr_in *client) {
    int sockfd;
    
    if((sockfd = socket_create_udp(port)) < 0) {
        return -1;
    }
    
    client->sin_port = htons(8888);

    if(connect(sockfd, (struct sockaddr *)client, sizeof(struct sockaddr)) < 0) {
        return -1;
    }

    return sockfd;
}
