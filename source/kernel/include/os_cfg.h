/*
 * @Author: warrior
 * @Date: 2023-07-13 15:49:49
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-16 17:25:31
 * @Description:
 */
#ifndef OS_CFG_H
#define OS_CFG_H

#define GDT_TABLE_SIZE 256            // GDT表项数量
#define KERNEL_SELECTOR_CS (1 * 8)    // 内核代码段描述符
#define KERNEL_SELECTOR_DS (2 * 8)    // 内核数据段描述符
#define SELECTOR_SYSCALL (3 * 8)      // 调用门描述符
#define KERNEL_STACK_SIZE (8 * 1024)  // 内核栈

#define OS_TICKS_MS 10

#define OS_VERSION "1.0.0"

#endif