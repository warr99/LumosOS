/*
 * @Author: warrior
 * @Date: 2023-07-13 14:57:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-06 16:14:00
 * @Description:
 */
#include "cpu/cpu.h"
#include "comm/cpu_instr.h"
#include "core/syscall.h"
#include "cpu/irq.h"
#include "ipc/mutex.h"
#include "os_cfg.h"

// 段描述符表(Descriptor Table - 全局描述符表 GDT)
static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t mutex;

void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr) {
    // 将selector(偏移量)转化为表项的指针
    // segment_desc_t* desc = gat_table + selector >> 3;
    segment_desc_t* desc = gdt_table + selector / sizeof(segment_desc_t);

    if (limit > 0xFFFFFF) {
        attr |= SEG_G;
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

int gdt_alloc_desc(void) {
    int i;

    // 跳过第0项
    mutex_lock(&mutex);
    for (i = 1; i < GDT_TABLE_SIZE; i++) {
        segment_desc_t* desc = gdt_table + i;
        if (desc->attr == 0) {
            desc->attr = SEG_P_PRESENT;  // 标记为占用状态
            break;
        }
    }
    mutex_unlock(&mutex);

    return i >= GDT_TABLE_SIZE ? -1 : i * sizeof(segment_desc_t);
}

int gdt_free_sel(int sel) {
    mutex_lock(&mutex);
    gdt_table[sel / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&mutex);
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

    gate_desc_set(
        (gate_desc_t*)(gdt_table + (SELECTOR_SYSCALL >> 3)),
        KERNEL_SELECTOR_CS,
        (uint32_t)exception_handler_syscall,
        GATE_P_PRESENT | GATE_DPL3 | GATE_TYPE_SYSCALL | SYSCALL_PARAM_COUNT);

    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

void cpu_init(void) {
    mutex_init(&mutex);
    gdt_init();
}

void switch_to_tss(int tss_sel) {
    far_jump(tss_sel, 0);
}