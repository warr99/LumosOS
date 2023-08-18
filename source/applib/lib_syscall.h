/*
 * @Author: warrior
 * @Date: 2023-07-31 21:43:32
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-11 14:38:08
 * @Description:
 */
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include <sys/stat.h>
#include "core/syscall.h"
#include "os_cfg.h"

typedef struct _syscall_args_t {
    int id;
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} syscall_args_t;

void msleep(int ms);

int getpid(void);

void print_msg(const char* fmt, int arg);

int fork(void);

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
int execve(const char* name, char* const* argv, char* const* env);

int yield(void);

int open(const char* name, int flags, ...);

int read(int file, char* ptr, int len);

int write(int file, char* ptr, int len);

int close(int file);

int lseek(int file, int ptr, int dir);

int isatty(int file);

int fstat(int file, struct stat* st);

void* sbrk(ptrdiff_t incr);

int dup(int file);

void _exit(int status);

int wait(int* status);

#endif