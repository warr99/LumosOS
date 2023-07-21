#ifndef SEM_H
#define SEM_H

#include "tools\list.h"

typedef struct _sem_t {
    int count;
    list_t wait_list;
} sem_t;

/**
 * @brief: 初始化临界资源信号量
 * @param {sem_t*} sem sem
 * @param {int} init_count 初始化值
 * @return {*}
 */
void sem_init(sem_t* sem, int init_count);

#endif