/*
 * @Author: warrior
 * @Date: 2023-07-18 10:36:04
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-19 10:26:03
 * @Description:
 */
#include "core/task.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

static int tss_init(task_t* task, uint32_t entry, uint32_t esp) {
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.\n");
        return -1;
    }
    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
                     SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);
    kernel_memset(&task->tss, 0, sizeof(tss_t));
    task->tss.eip = entry;
    task->tss.esp = task->tss.esp0 = esp;
    task->tss.ss = task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = KERNEL_SELECTOR_DS;
    task->tss.cs = KERNEL_SELECTOR_CS;
    task->tss.eflags = EFLAGS_IF | EFLAGS_DEFAULT;
    task->tss_sel = tss_sel;
    return 0;
}

int task_init(task_t* task, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t*)0);

    tss_init(task, entry, esp);
    // uint32_t* pesp = (uint32_t*)esp;
    // if (pesp) {
    //     *(--pesp) = entry;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     *(--pesp) = 0;
    //     task->stack=pesp;
    // }
    return 0;
}
/**
 * @brief: 任务切换
 * @param {uint32_t**} from 源任务的任务控制块中保存栈顶指针的地址,用于将源任务的栈顶指针保存到该地址中
 * @param {uint32_t*} to 目标任务的任务控制块中保存栈顶指针的值,用于跳转到目标任务的栈顶
 * @return {*}
 */
void simple_switch(uint32_t** from, uint32_t* to);

void task_switch(task_t* from, task_t* to) {
    switch_to_tss(to->tss_sel);
    //  simple_switch(&from->stack, to->stack);
}