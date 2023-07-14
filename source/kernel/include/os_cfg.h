/*
 * @Author: warrior
 * @Date: 2023-07-13 15:49:49
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-14 15:36:50
 * @Description: 
 */
#ifndef OS_CFG_H
#define OS_CFG_H

#define GDT_TABLE_SIZE 256            // GDT表项数量
#define KERNEL_SELECTOR_CS (1 * 8)    // 内核代码段描述符
#define KERNEL_SELECTOR_DS (2 * 8)    // 内核数据段描述符
#define KERNEL_STACK_SIZE (8 * 1024)  // 内核栈

#endif