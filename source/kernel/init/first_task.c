#include "applib/lib_syscall.h"
#include "core/task.h"
#include "tools/log.h"

int first_task_main(void) {
    int pid = getpid();
    for (;;) {
        print_msg("first task pid: %d",pid);
        msleep(1000);
    }
    return 0;
}   