/*************************************************************************
	> File Name: Wechat.h
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 02:22:48 PM CST
 ************************************************************************/

#ifndef _WECHAT_H
#define _WECHAT_H

#define MAXUSER 1000

/*
SYN：同步序列编号（Synchronize Sequence Numbers）。是TCP/IP建立连接时使用的握手信号。
在客户机和服务器之间建立正常的TCP网络连接时，客户机首先发出一个SYN消息，
服务器使用SYN+ACK应答表示接收到了这个消息，最后客户机再以ACK消息响应。
这样在客户机和服务器之间才能建立起可靠的TCP连接，数据才可以在客户机和服务器之间传递。

SYN：标志位，同步序列号
ACK：确认序号
FIN：断开连接请求
*/
#define CHAT_SYN 0x001
#define CHAT_ACK 0x002
#define CHAT_FIN 0x004
#define CHAT_HEART 0x008
// CHAT_MSG 表示心跳正常发的信息
#define CHAT_MSG 0x010

struct Msg {
    int type;
    char from[20];
    char to[20];
    char msg[1024];
};

#define FL_SYNSENT 0x001 
#define FL_ONLINE 0x002 

struct User {
    char name[20];
    int flag;  // 判断是否在线  0：不在线  1：在线
    int cnt;
    int fd;
};
#endif
