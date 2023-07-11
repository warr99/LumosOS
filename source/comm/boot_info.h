/*
 * @Author: warrior
 * @Date: 2023-07-11 14:46:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-11 18:05:53
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

#endif
