/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-18 09:39:31
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

void kernel_init(boot_info_t* boot_info) {
    ASSERT(boot_info->ram_region_count != 0);
    cpu_init();
    log_init();
    irq_init();
    time_init();
}

void init_task_entry(void) {
    int count = 0;
    for (;;) {
        log_printf("int main: %d", count++);
    }
}

void init_main(void) {
    log_printf("Kernel is running ...");

    int count = 0;
    for (;;) {
        log_printf("int main: %d", count++);
    }

    init_task_entry();
}