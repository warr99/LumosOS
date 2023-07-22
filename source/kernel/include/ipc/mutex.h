#ifndef MUTEX_H
#define MUTEX_H

#include "core/task.h"
#include "tools/list.h"

typedef struct _mutex_t {
    task_t* owner;     // 当前锁拥有者
    list_t wait_list;  // 等待队列
    int locked_count;  // 上锁次数
} mutex_t;

/**
 * @brief 互斥锁初始化
 * @param {mutex_t*} mutex 互斥锁
 * @return {*}
 */
void mutex_init(mutex_t* mutex);

/**
 * @brief 上锁
 * @param {mutex_t*} mutex 互斥锁
 * @return {*}
 */
void mutex_lock(mutex_t* mutex);

/**
 * @brief 解锁
 * @param {mutex_t*} mutex 互斥锁
 * @return {*}
 */
void mutex_unlock(mutex_t* mutex);

#endif