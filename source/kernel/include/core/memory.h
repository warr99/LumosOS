/*
 * @Author: warrior
 * @Date: 2023-07-22 09:30:44
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-05 23:53:42
 * @Description:
 */
#ifndef MEMORY_H
#define MEMORY_H

#include "comm/boot_info.h"
#include "comm/types.h"
#include "ipc/mutex.h"
#include "tools/bitmap.h"

#define MEM_EXT_START (1024 * 1024)
#define MEM_PAGE_SIZE 4096
#define MEM_EBDA_START 0x00080000
#define MEM_TASK_BASE 0x80000000
#define MEM_EXT_END (127 * 1024 * 1024)

/**
 * @brief 虚拟地址-物理地址映射关系表
 */
typedef struct _addr_alloc_t {
    mutex_t mutex_t;     // 互斥信号量
    bitmap_t bitmap;     // 表示内存使用情况的位图
    uint32_t start;      // 被管理的内存起始地址
    uint32_t size;       // 被管理的内存大小
    uint32_t page_size;  // 页大小
} addr_alloc_t;

typedef struct _memory_map_t {
    void* vstart;   // 逻辑起始地址
    void* vend;     // 逻辑结束地址
    void* pstart;   // 物理起始地址
    uint32_t perm;  // 特权属性
} memory_map_t;

/**
 * @brief 虚拟地址-物理地址映射关系表初始化
 * @param {boot_info_t*} boot_info 启动信息参数
 * @return {*}
 */
void memory_init(boot_info_t* boot_info);

/**
 * @brief 展示内存信息
 * @param {boot_info_t*} boot_info 启动信息
 * @return {*}
 */
void show_memory_info(boot_info_t* boot_info);

/**
 * @brief
 * @return {*}
 */
uint32_t memory_create_uvm(void);

int memory_alloc_page_for(uint32_t addr, uint32_t size, int perm);

/**
 * @brief 分配一页内存
 * @return {*} 起始地址
 */
uint32_t memory_alloc_one_page(void);

/**
 * @brief 释放一页内存
 * @return {*}
 */
void memory_free_one_page(uint32_t addr);

void memory_destroy_uvm(uint32_t page_dir);

uint32_t memory_copy_uvm(uint32_t page_dir);

#endif