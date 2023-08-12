/*
 * @Author: warrior
 * @Date: 2023-07-29 22:14:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-12 10:28:03
 * @Description:
 */
#ifndef SYSCALL_H
#define SYSCALL_H

#include "comm/types.h"

#define SYSCALL_PARAM_COUNT 5
#define SYS_sleep 0
#define SYS_getpid 1

#define SYS_fork 3
#define SYS_execve 4
#define SYS_yield 5

#define SYS_open 50
#define SYS_read 51
#define SYS_write 52
#define SYS_close 53
#define SYS_lseek 54

#define SYS_printmsg 100

/**
 * 系统调用的栈信息
 */
typedef struct _syscall_frame_t {
    int eflags;
    int gs, fs, es, ds;
    uint32_t edi, esi, ebp, dummy, ebx, edx, ecx, eax;
    int eip, cs;
    int func_id, arg0, arg1, arg2, arg3;
    int esp, ss;
} syscall_frame_t;

void exception_handler_syscall(void);

#endif