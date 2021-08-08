/*************************************************************************
	> File Name: udp_client.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 10:15:19 AM CST
 ************************************************************************/

#ifndef _UDP_CLIENT_H
#define _UDP_CLIENT_H
int socket_udp();
int udp_connect(struct sockaddr_in *client);
#endif
