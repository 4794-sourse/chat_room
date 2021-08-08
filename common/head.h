/*************************************************************************
	> File Name: head.h
	> Author:fangsong
	> Mail: 
	> Created Time: 2020年03月29日 星期日 22时33分32秒
 ************************************************************************/

#ifndef _HEAD_H
#define _HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/sem.h>
#include <curses.h>
// ---
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/ioctl.h>
#include <math.h>
#include <ctype.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include <locale.h>
//#include <cjson/cJSON.h>

#include "color.h"
//#include "tcp_client.h"
//#include "tcp_server.h"
#include "common.h"
#include "wechat.h"
#include "udp_server.h"
#include "udp_client.h"
#include "reactor.h"
#include "client_discover.h"
#include "heart_beat.h"
#include "send_chat.h"
//#include "chat_ui.h"

#ifdef _D
#define DBG(fmt, args...) printf(fmt, ##args)
#else
#define DBG(fmt, args...)
#endif

#endif
