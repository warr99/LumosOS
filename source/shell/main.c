/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-12 16:36:02
 * @Description:
 */
#include <stdio.h>
#include "lib_syscall.h"

int main(int argc, char** argv) {
    sbrk(0);
    sbrk(100);
    sbrk(200);
    sbrk(4096 * 2 + 100);
    printf("hello from x86 os\n");
    printf("os version: %s\n", OS_VERSION);
    printf("author: ChenXr\n");
    printf("create data: 2023-08-12 16:34:14\n");
    printf("%d...%d...%d...\n", 1, 2, 3);
    for (int i = 0; i < argc; i++) {
        print_msg("arg: %s", (int)argv[i]);
    }
    fork();
    yield();
    for (;;) {
        print_msg("shell pid: %d", getpid());
        msleep(1000);
    }
}