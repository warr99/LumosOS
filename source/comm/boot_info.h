/*
 * @Author: warrior
 * @Date: 2023-07-11 14:46:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-14 13:26:15
 * @Description:
 */
#ifndef BOOT_INFO_H
#define BOOT_INFO_H

#include "types.h"

#define BOOT_RAM_REGION_MAX 10  // RAM区最大数量

/**
 * 启动信息参数
 */
typedef struct _boot_info_t {
    // RAM区信息
    struct {
        uint32_t start;
        uint32_t size;
    } ram_region_cfg[BOOT_RAM_REGION_MAX];
    int ram_region_count;
} boot_info_t;

#define SECTOR_SIZE 512
#define SYS_KERNEL_LOAD_ADDR (1024 * 1024)

#endif
