/*************************************************************************
	> File Name: udp_server.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 09:57:25 AM CST
 ************************************************************************/

#ifndef _UDP_SERVER_H
#define _UDP_SERVER_H

int socket_create_udp(int port);
int udp_accept(int fd, struct User *user);

#endif
