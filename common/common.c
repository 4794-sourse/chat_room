/*************************************************************************
	> File Name: common.c
	> Author: 
	> Mail: 
	> Created Time: Sun 18 Apr 2021 10:33:39 AM CST
 ************************************************************************/

#include "head.h"

char conf_value_ans[512] = {0};

int make_nonblock(int fd) {
    int flag;
    if((flag = fcntl(fd, F_GETFL)) < 0) {
        return -1;
    }
    flag |= O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag);
}

int make_block(int fd) {
    int flag;
    if((flag = fcntl(fd, F_GETFL)) < 0) {
        return -1;
    }
    flag &= ~O_NONBLOCK;
    return fcntl(fd, F_SETFL, flag);
}

// 在路径为 path 的文件中找键为 key 的值
char *get_value(const char *path, const char *key) {
    FILE *fp = NULL;
    char *line = NULL, *sub = NULL;
    ssize_t nread, len;     // ssize_t: signed int 类型
    if(path == NULL || key == NULL) {
        fprintf(stderr, "argument error!\n");
        exit(1);
    }
    if((fp = fopen(path, "r")) == NULL) {
        fprintf(stderr, "file open error!\n");
        exit(1);
    }
    // getline（）从流中读取整行，并将包含文本的缓冲区地址存储到* line 中。
    // 如果找到缓冲区，则该缓冲区以null终止，并包括换行符。
    // 成功时，getline（）返回读取的字符数，包括除数字符，但不包括终止的空字节（'\ 0'）
    // 如果无法读取行（包括文件结束条件），则函数将返回-1。 如果发生错误，则设置errno来指示原因。 
    while((nread = getline(&line, &len, fp)) != -1) {
        // strstr(line, key):在line中查找首次出现key的位置
        // 找到则返回一个指向所定位子字符串开头的指针，未找到则返回NULL
        // 如果在 line 中没有找到 key 子串，那就 continue
        if((sub = strstr(line, key)) == NULL) continue;
        if(line[strlen(key)] == '=' && sub == line) {
            // 把 '=' 后面的内容赋给 conf_value_ans
            // 例: sub='name=ccc' line='name=ccc' conf_value_ans='ccc'
            strcpy(conf_value_ans, line + strlen(key) + 1);
            /* printf("%s%d", conf_value_ans, strlen(conf_value_ans));
            结果：ccc (结尾有换行符)
                  4
            */
            conf_value_ans[strlen(conf_value_ans) - 1] = '\0';
        }
    }
    free(line);
    fclose(fp);
    return conf_value_ans;
}
