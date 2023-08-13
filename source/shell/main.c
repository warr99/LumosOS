/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-13 22:19:16
 * @Description:
 */
#include <stdio.h>
#include "lib_syscall.h"

int main(int argc, char** argv) {
    printf("Welcome \033[33;49mLumosOS\n");
    printf("\033[39;49m---------------\n");
    printf("VERSION: %s\n", OS_VERSION);
    printf("Author: ChenXr\n");
    printf("Create Data: 2023-07\n");
    // printf("abef\b\b\b\bcd\n");  // \b: 输出cdef
    // printf("abcd\x7f;fg\n");     // 7f: 输出 abc;fg
    // printf("\0337Hello, world!\0338HHHHH\n");
    // printf("\033[34;49mHello world!\033[39;49m by LumosOS\n");
    for (int i = 0; i < argc; i++) {
        printf("arg: %s\n", (int)argv[i]);
    }
    fork();
    yield();
    for (;;) {
        printf("shell pid: %d\n", getpid());
        msleep(1000);
    }
}