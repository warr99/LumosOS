/*
 * @Author: warrior
 * @Date: 2023-07-13 14:49:44
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-13 16:13:19
 * @Description:
 */
#ifndef CPU_H
#define CPU_H

#include "comm/types.h"

#pragma pack(1)
typedef struct _segment_desc_t {
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
} segment_desc_t;

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);

void cpu_init(void);

#endif