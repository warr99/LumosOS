/*
 * @Author: warrior
 * @Date: 2023-08-15 10:31:50
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-16 13:47:45
 * @Description:
 */
#ifndef TTY_H
#define TTY_H

#include "ipc/sem.h"

#define TTY_NR 8           // 最大支持的tty设备数量
#define TTY_IBUF_SIZE 512  // tty输入缓存大小
#define TTY_OBUF_SIZE 512  // tty输出缓存大小

#define TTY_OCRLF (1 << 0)

// 表示FIFO（First In, First Out）缓存
typedef struct _tty_fifo_t {
    char* buf;
    int size;         // 最大字节数
    int read, write;  // 当前读写位置
    int count;        // 当前已有的数据量
} tty_fifo_t;

// tty设备
typedef struct _tty_t {
    char obuf[TTY_OBUF_SIZE];
    tty_fifo_t ofifo;  // 输出缓存
    sem_t osem;
    char ibuf[TTY_IBUF_SIZE];
    tty_fifo_t ififo;  // 输入缓存
    int console_index;
    int oflags;
} tty_t;

int tty_fifo_get(tty_fifo_t* fifo, char* c);

int tty_fifo_put(tty_fifo_t* fifo, char c);

#endif