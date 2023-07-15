/*
 * @Author: warrior
 * @Date: 2023-07-15 09:49:17
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-15 16:07:03
 * @Description: 中断处理相关
 */
#ifndef IRQ_H
#define IRQ_H

#define IRQ0_DE 0

#include "comm/types.h"

typedef struct _exception_frame_t {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, error_code;
    uint32_t eip, cs, eflags;
} exception_frame_t;

/**
 * @description: 定义了一个函数指针类型 irq_handler_t,
 * 指向一个参数为 exception_frame_t* 类型的函数
 * 这意味着该函数类型可以用于处理异常
 * @param {exception_frame_t*} frame exception_frame_t
 * @return {*}
 */
typedef void (*irq_handler_t)(exception_frame_t* frame);

/**
 * @description: 初始化中断向量表
 * @return {void}
 */
void irq_init(void);

void exception_handler_divider(void);

int irq_install(int irq_num, irq_handler_t handler);

#endif
