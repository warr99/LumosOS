/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-27 16:30:00
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "comm/cpu_instr.h"
#include "core/memory.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/console.h"
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
    console_init();
    
    memory_init(boot_info);
    irq_init();
    time_init();

    task_manager_init();
}

/**
 * @brief 移至第一个进程运行
 * @return {*}
 */
void move_to_first_task(void) {
    task_t* curr = task_current();
    ASSERT(curr != 0);
    tss_t* tss = &(curr->tss);
    __asm__ __volatile__(
        "push %[ss]\n\t"      // SS
        "push %[esp]\n\t"     // ESP
        "push %[eflags]\n\t"  // EFLAGS
        "push %[cs]\n\t"      // CS
        "push %[eip]\n\t"     // ip
        "iret\n\t"
        :
        : [ss] "r"(tss->ss),
          [esp] "r"(tss->esp),
          [eflags] "r"(tss->eflags),
          [cs] "r"(tss->cs),
          [eip] "r"(tss->eip));
}

void init_main(void) {
    log_printf("Kernel is running ...");
    task_first_init();
    move_to_first_task();
}