/*
 * @Author: warrior
 * @Date: 2023-07-15 09:50:54
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-15 16:10:06
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

    irq_install(IRQ0_DE, (irq_handler_t)exception_handler_divider);

    lidt((uint32_t)idt_table, sizeof(idt_table));
}

int irq_install(int irq_num, irq_handler_t handler) {
    if (irq_num >= IDT_TABLE_NR) {
        return -1;
    }
    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, (uint32_t)handler, GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);
    return 0;
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
    do_default_handler(frame, "Unknown Exception");
}

void do_handler_divider(exception_frame_t* frame) {
    do_default_handler(frame, "Divider Exception");
}
