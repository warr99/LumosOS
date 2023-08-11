/*
 * @Author: warrior
 * @Date: 2023-07-31 21:43:32
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-11 10:48:46
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
    uint32_t addr[] = {0, SELECTOR_SYSCALL | 0};
    int ret;
    __asm__ __volatile__(
        "push %[arg3]\n\t"
        "push %[arg2]\n\t"
        "push %[arg1]\n\t"
        "push %[arg0]\n\t"
        "push %[id]\n\t"
        "lcalll *(%[a])"
        : "=a"(ret)
        : [arg3] "r"(args->arg3),
          [arg2] "r"(args->arg2),
          [arg1] "r"(args->arg1),
          [arg0] "r"(args->arg0),
          [id] "r"(args->id),
          [a] "r"(addr));
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

static inline int fork(void) {
    syscall_args_t args;
    args.id = SYS_fork;
    sys_call(&args);
}

/**
 * @brief 执行指定的程序文件并替换当前进程映像。
 *
 * @param name  要执行的程序的文件路径。
 * @param argv      一个字符串数组，用于传递给新程序的命令行参数。
 *                  数组中的第一个元素通常是新程序的名称，后续元素是传递给新程序的参数。
 * @param env      一个字符串数组，用于传递给新程序的环境变量。
 *                  这是一个包含"key=value"格式的字符串的数组，用于设置新程序的环境。
 *
 * @return          如果调用成功，函数不会返回；如果发生错误，将返回-1，并设置errno来指示错误类型。
 */
static inline int execve(const char* name, char* const* argv, char* const* env) {
    syscall_args_t args;
    args.id = SYS_execve;
    args.arg0 = (int)name;
    args.arg1 = (int)argv;
    args.arg2 = (int)env;
    return sys_call(&args);
}

static inline int yield(void) {
    syscall_args_t args;
    args.id = SYS_yield;
    sys_call(&args);
}

#endif