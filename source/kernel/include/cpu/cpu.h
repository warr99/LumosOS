/*
 * @Author: warrior
 * @Date: 2023-07-13 14:49:44
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-15 10:38:03
 * @Description:
 */
#ifndef CPU_H
#define CPU_H

#include "comm/types.h"

#pragma pack(1)

// GDT 表项
typedef struct _segment_desc_t {
    uint16_t limit15_0;
    uint16_t base15_0;
    uint8_t base23_16;
    uint16_t attr;
    uint8_t base31_24;
} segment_desc_t;

// 中断门表项
typedef struct _gate_desc_t {
    uint16_t offset15_0;
    uint16_t selector;
    uint16_t attr;
    uint16_t offset31_16;
} gate_desc_t;

#pragma pack()

#define SEG_G (1 << 15)         // 设置段界限的单位，1-4KB，0-字节
#define SEG_D (1 << 14)         // 控制是否是32位、16位的代码或数据段
#define SEG_P_PRESENT (1 << 7)  // 段是否存在

#define SEG_DPL0 (0 << 5)  // 特权级0，最高特权级
#define SEG_DPL3 (3 << 5)  // 特权级3，最低权限

#define SEG_S_SYSTEM (0 << 4)  // 是否是系统段，如调用门或者中断
#define SEG_S_NORMAL (1 << 4)  // 普通的代码段或数据段

#define SEG_TYPE_CODE (1 << 3)  // 指定其为代码段
#define SEG_TYPE_DATA (0 << 3)  // 数据段

#define SEG_TYPE_RW (1 << 1)  // 是否可写可读，不设置为只读

#define SEG_TYPE_TSS (9 << 0)  // 32位TSS

#define GATE_TYPE_IDT (0xE << 8)      // 中断32位门描述符
#define GATE_TYPE_SYSCALL (0xC << 8)  // 调用门
#define GATE_P_PRESENT (1 << 15)      // 是否存在
#define GATE_DPL0 (0 << 13)           // 特权级0，最高特权级
#define GATE_DPL3 (3 << 13)           // 特权级3，最低权限

#define SEG_RPL0 (0 << 0)
#define SEG_RPL3 (3 << 0)

#define EFLAGS_IF (1 << 9)
#define EFLAGS_DEFAULT (1 << 1)

#pragma pack(1)

/**
 * @brief: 设置 GDT 表项
 * @param {int} selector 表项的偏移量(8,16...)
 * @param {uint32_t} base 对应的内存区域的起始地址
 * @param {uint32_t} limit 对应的内存区域的大小
 * @param {uint16_t} attr 其他参数(参见手册)
 * @return {*} void
 */
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr);

/**
 * @brief: 设置 中断向量表 表项
 * @param {gate_desc_set} *desc 要设置的表项的指针
 * @param {uint16_t} selector 对应的 GDT 表的选择子
 * @param {uint32_t} offset 偏移量
 * @param {uint16_t} attr 属性
 * @return {*} void
 */
void gate_desc_set(gate_desc_t* desc, uint16_t selector, uint32_t offset, uint16_t attr);

void cpu_init(void);

#endif