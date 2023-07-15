/*
 * @Author: warrior
 * @Date: 2023-07-15 09:50:54
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-15 14:26:59
 * @Description:
 */
#include "cpu/irq.h"
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "os_cfg.h"

#define IDT_TABLE_NR 128

void exception_handler_unknown(void);

/**
 * 中断向量表
 */
static gate_desc_t idt_table[IDT_TABLE_NR];

void irq_init(void) {
    for (int i = 0; i < IDT_TABLE_NR; i++) {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_unknown, GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);
    }
    lidt((uint32_t)idt_table, sizeof(idt_table));
}

/**
 * @description:
 * @param {exception_frame_t*} frame
 * @param {char*} message 错误信息
 * @return {*}
 */
static void do_default_handler(exception_frame_t* frame, const char* message) {
    for (;;) {
    }
}

void do_handler_unknown(exception_frame_t* frame) {
    do_default_handler(frame, "unknown exception");
}