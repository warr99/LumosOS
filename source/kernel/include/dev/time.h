/*
 * @Author: warrior
 * @Date: 2023-07-16 12:09:42
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-16 17:26:47
 * @Description:
 */
#ifndef TIMER_H
#define TIMER_H

#include "comm/types.h"

#define PIT_OSC_FREQ 1193182  // 定时器时钟

// 定时器的寄存器和各项位配置
#define PIT_CHANNEL0_DATA_PORT 0x40
#define PIT_COMMAND_MODE_PORT 0x43

#define PIT_CHANNLE0 (0 << 6)
#define PIT_LOAD_LOHI (3 << 4)
#define PIT_MODE3 (3 << 1)

/**
 * @description: 初始化定时器
 * @return {*}
 */
void time_init(void);

/**
 * @description: 中断处理函数
 * @return {*}
 */
void exception_handler_timer(void);

#endif