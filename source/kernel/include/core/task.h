/*
 * @Author: warrior
 * @Date: 2023-07-18 10:29:35
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-25 11:50:39
 * @Description:
 */
#ifndef TASK_H
#define TASK_H

#include "comm/types.h"
#include "cpu/cpu.h"
#include "tools/list.h"

#define TASK_NAME_SIZE 32
#define TASK_TIME_SLICE_DEFAULT 10

typedef struct _task_t {
    enum {
        TASK_CREATED,
        TASK_RUNNING,
        TASK_SLEEP,
        TASK_READY,
        TASK_WAITTING,
    } state;  // 任务状态
    int time_ticks;
    int slice_ticks;
    int sleep_ticks;            // 延时时长
    char name[TASK_NAME_SIZE];  // 任务名称(唯一标识)
    list_node_t run_node;       // 作为就绪队列的节点
    list_node_t all_node;       // 作为进程队列的节点
    list_node_t wait_node;      // 作为等待队列的节点
    tss_t tss;
    int tss_sel;
} task_t;

typedef struct _task_manager_t {
    task_t* curr_task;

    list_t ready_list;  // 就绪队列
    list_t task_list;   // 进程队列
    list_t sleep_list;  // 睡眠队列

    task_t first_task;
    task_t idle_task;  // 空闲进程

    int app_code_sel;  // 代码段选择子
    int app_data_sel;  // 数据段选择子
} task_manager_t;

/**
 * @brief 任务管理器初始化
 * @return {*}
 */
void task_manager_init(void);

/**
 * @brief 初始化 task_manager_t->first_task
 * @return {*}
 */
void task_first_init(void);

/**
 * @brief 取出 task_manager_t->first_task
 * @return {task_t*} first_task
 */
task_t* get_first_task(void);

/**
 * @brief
 * @param {task_t*} 对应的任务控制块结构
 * @param {char*} 任务名称
 * @param {uint32_t} entry 任务入口
 * @param {uint32_t} esp 任务对应的栈顶指针
 * @return {*}
 */
int task_init(task_t* task, const char* name, uint32_t entry, uint32_t esp);

void task_switch(task_t* from, task_t* to);

/**
 * @brief 将任务插入就绪队列并修改其状态
 * @param {task_t*} task
 * @return {*}
 */
void task_set_ready(task_t* task);

/**
 * @brief 将任务从就绪队列中移除
 * @param {task_t*} task
 * @return {*}
 */
void task_set_block(task_t* task);

/**
 * @brief 使得当前任务主动让出CPU
 * @return {*} 0
 */
int sys_sched_yield(void);

void task_dispatch(void);

task_t* task_current(void);

void task_time_tick(void);

void task_dispatch(void);

void task_set_sleep(task_t* task, uint32_t ticks);

void task_set_wakeup(task_t* task);

void sys_sleep(uint32_t ms);

#endif
