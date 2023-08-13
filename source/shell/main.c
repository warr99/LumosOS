/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-13 15:19:27
 * @Description:
 */
#include <stdio.h>
#include "lib_syscall.h"

int main(int argc, char** argv) {
    printf("LumosOS\n");
    printf("OS VERSION: %s\n", OS_VERSION);
    printf("Author: ChenXr\n");
    printf("Create Data: 2023-08\n");
    // printf("abef\b\b\b\bcd\n");  // \b: 输出cdef
    // printf("abcd\x7f;fg\n");     // 7f: 输出 abc;fg
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