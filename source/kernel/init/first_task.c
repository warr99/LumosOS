#include "applib/lib_syscall.h"
#include "core/task.h"
#include "tools/log.h"

int first_task_main(void) {
    int pid = getpid();
    for (;;) {
        // log_printf("first task");
        // sys_sleep(1000);
        msleep(1000);
    }
    return 0;
}