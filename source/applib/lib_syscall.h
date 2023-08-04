/*
 * @Author: warrior
 * @Date: 2023-07-31 21:43:32
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-04 16:50:41
 * @Description:
 */
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "core/syscall.h"
#include "os_cfg.h"

typedef struct _syscall_args_t {
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} syscall_args_t;

static inline int sys_call(syscall_args_t* args) {
    int ret;
    __asm__ __volatile__(
        "int $0x80"
        : "=a"(ret)
        : "S"(args->arg3),
          "d"(args->arg2),
          "c"(args->arg1),
          "b"(args->arg0),
          "a"(args->id));
    return ret;
}

static inline void msleep(int ms) {
    if (ms <= 0) {
        return;
    }
    syscall_args_t args;
    args.id = SYS_sleep;
    args.arg0 = ms;
    sys_call(&args);
}

static inline int getpid(void) {
    syscall_args_t args;
    args.id = SYS_getpid;
    return sys_call(&args);
}

static inline void print_msg(const char* fmt, int arg) {
    syscall_args_t args;
    args.id = SYS_printmsg;
    args.arg0 = (int)fmt;
    args.arg1 = arg;
    sys_call(&args);
}

#endif