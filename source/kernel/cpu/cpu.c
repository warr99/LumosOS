/*
 * @Author: warrior
 * @Date: 2023-07-13 14:57:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-21 09:37:21
 * @Description:
 */
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "os_cfg.h"

static segment_desc_t gdt_table[GDT_TABLE_SIZE];

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr) {
    // 将selector(偏移量)转化为表项的指针
    // segment_desc_t* desc = gat_table + selector >> 3;
    segment_desc_t* desc = gdt_table + selector / sizeof(segment_desc_t);

    if (limit > 0xFFFFFF) {
        attr | SEG_G;
        limit /= 0x100;
    }

    desc->limit15_0 = limit & 0xFFFF;
    desc->base15_0 = base & 0xFFFF;
    desc->base23_16 = (base >> 16) & 0xFF;
    desc->attr = attr | (((limit >> 16) & 0xF) << 8);
    desc->base31_24 = (base >> 24) & 0xFF;
}

void gate_desc_set(gate_desc_t* desc, uint16_t selector, uint32_t offset, uint16_t attr) {
    desc->offset15_0 = offset & 0xFFFF;
    desc->selector = selector;
    desc->attr = attr;
    desc->offset31_16 = (offset >> 16) & 0xFFFF;
}

int gdt_alloc_desc() {
    irq_state_t state = irq_enter_protection();
    for (int i = 1; i < GDT_TABLE_SIZE; i++) {
        segment_desc_t* desc = gdt_table + i;
        if (desc->attr == 0) {
            irq_leave_protection(state);
            return i * sizeof(segment_desc_t);
        }
    }
    irq_leave_protection(state);
    return -1;
}

void gdt_init(void) {
    for (int i = 0; i < GDT_TABLE_SIZE; i++) {
        segment_desc_set(i * sizeof(segment_desc_t), 0, 0, 0);
    }

    // 只能用非一致代码段，以便通过调用门更改当前任务的CPL执行关键的资源访问操作
    segment_desc_set(KERNEL_SELECTOR_CS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D | SEG_G);

    // 数据段
    segment_desc_set(KERNEL_SELECTOR_DS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D | SEG_G);
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init(void) {
    gdt_init();
}

void switch_to_tss(int tss_sel) {
    far_jump(tss_sel, 0);
}