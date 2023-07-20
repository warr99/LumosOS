/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-20 10:31:14
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
}

static task_t first_task;
static task_t init_task;
static uint32_t init_task_stack[1024];

void init_task_entry(void) {
    int count = 0;
    for (;;) {
        log_printf("init task: %d", count++);
        task_switch(&init_task, get_first_task());
    }
}

void init_main(void) {
    log_printf("Kernel is running ...");

    task_init(&init_task, "init task", (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_first_init();

    int count = 0;
    for (;;) {
        log_printf("init main: %d", count++);
        task_switch(get_first_task(), &init_task);
    }

    init_task_entry();
}