/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-21 10:34:15
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "comm/cpu_instr.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
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

void init_task_entry(void) {
    int count = 0;
    for (;;) {
        log_printf("-----init task----- %d", count++);
        sys_sleep(1000);
    }
}

void init_main(void) {
    log_printf("Kernel is running ...");

    task_init(&init_task, "init_task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_first_init();

    irq_enable_global();

    int count = 0;
    for (;;) {
        log_printf("=====init main===== %d", count++);
        sys_sleep(1000);
    }

    init_task_entry();
}