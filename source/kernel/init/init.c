/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-21 15:00:49
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "comm/cpu_instr.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "ipc/sem.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/list.h"
#include "tools/log.h"

void kernel_init(boot_info_t* boot_info) {
    ASSERT(boot_info->ram_region_count != 0);
    cpu_init();
    log_init();
    irq_init();
    time_init();
    task_manager_init();
}

static task_t first_task;
static task_t init_task;
static uint32_t init_task_stack[1024];
static sem_t sem;

void init_task_entry(void) {
    int count = 0;
    for (;;) {
        // sem_wait(&sem);
        log_printf(">>>> init_task_entry()_%d <<<<", count++);
    }
}

void init_main(void) {
    log_printf("Kernel is running ...");

    task_init(&init_task, "init_task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_first_init();
    sem_init(&sem, 0);
    irq_enable_global();

    int count = 0;
    for (;;) {
        log_printf(">>>> init_main()_%d <<<<", count++);
        // if (count % 5 == 0) {
        //     sem_notify(&sem);
        // }
        // sys_sleep(1000);
    }

    // init_task_entry();
}