#ifndef SEM_H
#define SEM_H

#include "tools\list.h"

typedef struct _sem_t {
    int count;
    list_t wait_list;
} sem_t;

/**
 * @brief: 初始化信号量
 * @param {sem_t*} sem sem
 * @param {int} init_count 初始化值
 * @return {*}
 */
void sem_init(sem_t* sem, int init_count);

/**
 * @brief: 等信号
 * @param {sem_t*} sem 信号量
 * @return {*}
 */
void sem_wait(sem_t* sem);

/**
 * @brief: 发信号
 * @param {sem_t*} sem 信号量
 * @return {*}
 */
void sem_notify(sem_t* sem);

/**
 * @brief: 返回count
 * @param {sem_t*} sem 信号量
 * @return {int} count
 */
int sem_count(sem_t* sem);

#endif