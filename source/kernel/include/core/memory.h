#ifndef MEMORY_H
#define MEMORY_H

#include "comm/types.h"
#include "comm/boot_info.h"
#include "ipc/mutex.h"
#include "tools/bitmap.h"

/**
 * @brief 虚拟地址-物理地址映射关系表
*/
typedef struct _addr_alloc_t {
    mutex_t mutex_t;    // 互斥信号量
    bitmap_t bitmap;    // 表示内存使用情况的位图
    uint32_t start;     // 被管理的内存起始地址
    uint32_t size;      // 被管理的内存大小
    uint32_t page_size;  // 页大小
} addr_alloc_t;

/**
 * @brief 虚拟地址-物理地址映射关系表初始化
 * @param {boot_info_t*} boot_info 启动信息参数
 * @return {*}
 */
void memory_init(boot_info_t* boot_info);

#endif