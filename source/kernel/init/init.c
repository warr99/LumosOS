/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-16 16:41:56
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "tools/log.h"
#include "os_cfg.h"

void kernel_init(boot_info_t* boot_info) {
    cpu_init();
    log_init();
    irq_init();
    time_init();
}

void init_main(void) {
    log_printf("Kernel is running ...");
    log_printf("Version: %s", OS_VERSION);
    // int i = 3 / 0;
    // irq_enable_global();
    for (;;) {
    }
}