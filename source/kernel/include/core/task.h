/*
 * @Author: warrior
 * @Date: 2023-07-18 10:29:35
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-18 20:39:09
 * @Description:
 */
#ifndef TSAK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"

typedef struct _task_t {
    uint32_t* stack;
    tss_t tss;
    int tss_sel;
} task_t;

/**
 * @brief:
 * @param {task_t*} 对应的任务控制块结构
 * @param {uint32_t} entry 任务入口
 * @param {uint32_t} esp 任务对应的栈顶指针
 * @return {*}
 */
int task_init(task_t* task, uint32_t entry, uint32_t esp);

void task_switch(task_t* from, task_t* to);

#endif
