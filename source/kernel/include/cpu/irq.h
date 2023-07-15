/*
 * @Author: warrior
 * @Date: 2023-07-15 09:49:17
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-15 14:35:39
 * @Description: 中断处理相关
 */
#ifndef IRQ_H
#define IRQ_H

#include "comm/types.h"

typedef struct _exception_frame_t {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eip, cs, eflags;
} exception_frame_t;

/**
 * @description: 初始化中断向量表
 * @return {void}
 */
void irq_init(void);

#endif
