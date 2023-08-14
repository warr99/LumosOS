/*
 * @Author: warrior
 * @Date: 2023-07-11 21:07:04
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-14 14:37:26
 * @Description: 汇编指令封装
 */
#ifndef CPU_INSTR_H
#define CPU_INSTR_H

#include "types.h"

static inline uint8_t inb(uint16_t port) {
    uint8_t rv;
    __asm__ __volatile__("inb %[p], %[v]"
                         : [v] "=a"(rv)
                         : [p] "d"(port));
    return rv;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t rv;
    __asm__ __volatile__("in %[p], %[v]"
                         : [v] "=a"(rv)
                         : [p] "d"(port));
    return rv;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__("outb %[v], %[p]"
                         :
                         : [p] "d"(port), [v] "a"(data));
}

static inline void cli() {
    __asm__ __volatile__("cli");
}

static inline void sti() {
    __asm__ __volatile__("sti");
}

/**
 * @brief 用于加载全局描述符表（GDT），其中包含用于管理内存段的描述符信息。
 * @param  start GDT表的起始地址，表示一个32位无符号整数，指向GDT表在内存中的起始位置
 * @param  size GDT表的大小，表示一个32位无符号整数，指示GDT表所占用的字节数
 * @return {*}
 */
static inline void lgdt(uint32_t start, uint32_t size) {
    struct {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    } gdt;

    gdt.start31_16 = start >> 16;
    gdt.start15_0 = start & 0xFFFF;
    gdt.limit = size - 1;

    __asm__ __volatile__("lgdt %[g]" ::[g] "m"(gdt));
}

static inline void lidt(uint32_t start, uint32_t size) {
    struct {
        uint16_t limit;
        uint16_t start15_0;
        uint16_t start31_16;
    } idt;

    idt.start31_16 = start >> 16;
    idt.start15_0 = start & 0xFFFF;
    idt.limit = size - 1;

    __asm__ __volatile__("lidt %[g]" ::[g] "m"(idt));
}

static inline uint32_t read_cr0(void) {
    uint32_t cr0;
    __asm__ __volatile__("mov %%cr0, %[v]"
                         : [v] "=r"(cr0));
}

static inline void write_cr0(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr0"
                         :
                         : [v] "r"(v));
}

static inline void far_jump(uint32_t selector, uint32_t offset) {
    uint32_t addr[] = {offset, selector};
    __asm__ __volatile__("ljmpl *(%[a])"
                         :
                         : [a] "r"(addr));
}

static inline void hlt(void) {
    __asm__ __volatile__("hlt");
}

static inline void write_tr(uint16_t tll_sel) {
    __asm__ __volatile__("ltr %%ax"
                         :
                         : "a"(tll_sel));
}

static inline uint32_t read_eflags() {
    uint32_t eflags;
    __asm__ __volatile__("pushf\n\tpop %%eax"
                         : "=a"(eflags));
    return eflags;
}

static inline void write_eflags(uint32_t eflags) {
    __asm__ __volatile__("push %%eax\n\tpopf"
                         :
                         : "a"(eflags));
}

static inline uint32_t read_cr2(void) {
    uint32_t cr2;
    __asm__ __volatile__("mov %%cr2, %[v]"
                         : [v] "=r"(cr2));
}

static inline void write_cr2(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr2"
                         :
                         : [v] "r"(v));
}

static inline uint32_t read_cr3(void) {
    uint32_t cr3;
    __asm__ __volatile__("mov %%cr3, %[v]"
                         : [v] "=r"(cr3));
}

static inline void write_cr3(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr3"
                         :
                         : [v] "r"(v));
}

static inline uint32_t read_cr4(void) {
    uint32_t cr4;
    __asm__ __volatile__("mov %%cr4, %[v]"
                         : [v] "=r"(cr4));
}

static inline void write_cr4(uint32_t v) {
    __asm__ __volatile__("mov %[v], %%cr4"
                         :
                         : [v] "r"(v));
}

#endif