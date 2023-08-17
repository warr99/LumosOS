/*
 * @Author: warrior
 * @Date: 2023-07-25 10:10:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-17 16:40:20
 * @Description:
 */
#include "applib/lib_syscall.h"
#include "core/task.h"
#include "dev/tty.h"
#include "tools/log.h"

int first_task_main(void) {
#if 0
    int count = 3;

    int pid = getpid();
    print_msg("first task id=%d", pid);

    pid = fork();

    if (pid < 0) {
        print_msg("create child proc failed.", 0);
    } else if (pid == 0) {
        count += 3;
        print_msg("child: %d", count);
        char* argv[] = {
            "arg0",
            "arg1",
            "arg2",
            "arg3",
        };
        execve("/shell.elf", argv, (char**)0);
    } else {
        count += 1;
        print_msg("child task id=%d", pid);
        print_msg("parent: %d", count);
    }
    pid = getpid();
#endif
    for (int i = 0; i < TTY_NR; i++) {
        int pid = fork();
        if (pid < 0) {
            print_msg("create shell proc failed", 0);
            break;
        } else if (pid == 0) {
            // 子进程
            char tty_num[5] = "tty:?";
            tty_num[4] = i + '0';
            char* argv[] = {tty_num, (char*)0};
            execve("/shell.elf", argv, (char**)0);
            print_msg("create shell proc failed", 0);
            while (1) {
                msleep(10000);
            }
        }
    }

    while (1) {
        msleep(10000);
    }
    return 0;
}