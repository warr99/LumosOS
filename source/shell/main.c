/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-13 23:15:44
 * @Description:
 */
#include <stdio.h>
#include "lib_syscall.h"

int main(int argc, char** argv) {
    // printf("123\033[2DHello,word!\n");  // 光标左移2，1Hello,word!
    // printf("123\033[2CHello,word!\n");  // 光标右移2，123  Hello,word!

    // printf("\033[31m");             // ESC [pn m, Hello,world红色，其余绿色
    // printf("\033[10;10H test!\n");  // 定位到10, 10，test!
    // printf("\033[20;20H test!\n");  // 定位到20, 20，test!
    // printf("\033[32;25;39m123\n");  // ESC [pn m, Hello,world红色，其余绿色

    // printf("\033[39;49m--------------------------------------------------\n");

    printf("\033[2J\n");  // clear screen

    printf("Welcome \033[33;49mLumosOS\n");
    printf("\033[39;49m---------------\n");
    printf("VERSION: %s\n", OS_VERSION);
    printf("Author: ChenXr\n");
    printf("Create Data: 2023-07\n");
    // printf("abef\b\b\b\bcd\n");  // \b: 输出cdef
    // printf("abcd\x7f;fg\n");     // 7f: 输出 abc;fg
    // printf("\0337Hello, world!\0338HHHHH\n");
    // printf("\033[34;49mHello world!\033[39;49m by LumosOS\n");
    // for (int i = 0; i < argc; i++) {
    //     printf("arg: %s\n", (int)argv[i]);
    // }
    // fork();
    // yield();
    // for (;;) {
    //     printf("shell pid: %d\n", getpid());
    //     msleep(1000);
    // }
    for (;;) {
        msleep(1000);
    }
}