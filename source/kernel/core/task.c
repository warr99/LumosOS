/*
 * @Author: warrior
 * @Date: 2023-07-18 10:36:04
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-20 22:05:08
 * @Description:
 */
#include "core/task.h"
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

static task_mananger_t task_mananger;

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

int task_init(task_t* task, const char* name, uint32_t entry, uint32_t esp) {
    ASSERT(task != (task_t*)0);
    tss_init(task, entry, esp);
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    irq_state_t state = irq_enter_protection();
    task_set_ready(task);
    list_insert_last(&task_mananger.task_list, &task->all_node);
    irq_leave_protection(state);
    return 0;
}

/**
 * @brief: 任务切换
 * @param {uint32_t**} from 源任务的任务控制块中保存栈顶指针的地址,用于将源任务的栈顶指针保存到该地址中
 * @param {uint32_t*} to 目标任务的任务控制块中保存栈顶指针的值,用于跳转到目标任务的栈顶
 * @return {*}
 */
void simple_switch(uint32_t** from, uint32_t* to);

void task_mananger_init(void) {
    list_init(&task_mananger.ready_list);
    list_init(&task_mananger.task_list);

    task_mananger.curr_task = (task_t*)0;
}

task_t* get_first_task(void) {
    return &task_mananger.first_task;
}

void task_first_init(void) {
    task_init(&task_mananger.first_task, "first task", (uint32_t)0, 0);
    write_tr(task_mananger.first_task.tss_sel);
    task_mananger.curr_task = &task_mananger.first_task;
}

void task_switch(task_t* from, task_t* to) {
    switch_to_tss(to->tss_sel);
    //  simple_switch(&from->stack, to->stack);
}

void task_set_ready(task_t* task) {
    list_insert_last(&task_mananger.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block(task_t* task) {
    list_remove(&task_mananger.ready_list, &task->run_node);
}

task_t* task_current(void) {
    return task_mananger.curr_task;
}

int sys_sched_yield(void) {
    irq_state_t state = irq_enter_protection();
    if (list_count(&task_mananger.ready_list) > 1) {
        task_t* curr_task = task_current();
        task_set_block(curr_task);
        task_set_ready(curr_task);

        task_dispatch();
    }
    irq_leave_protection(state);
    return 0;
}

task_t* task_next_run(void) {
    list_node_t* task_node = list_first(&task_mananger.ready_list);
    return list_node_parent(task_node, task_t, run_node);
}

void task_dispatch(void) {
    irq_state_t state = irq_enter_protection();
    task_t* to = task_next_run();
    if (to != task_mananger.curr_task) {
        task_t* from = task_current();
        task_mananger.curr_task = to;
        to->state = TASK_RUNNING;
        task_switch(from, to);
    }
    irq_leave_protection(state);
}

void task_time_tick(void) {
    task_t* curr_task = task_current();
    if (--curr_task->slice_ticks == 0) {
        curr_task->slice_ticks = curr_task->time_ticks;
        task_set_block(curr_task);
        task_set_ready(curr_task);
        task_dispatch();
    }
}
