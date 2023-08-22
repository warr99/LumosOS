/*
 * @Author: warrior
 * @Date: 2023-07-15 09:49:17
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-14 11:00:33
 * @Description: 中断处理相关
 */
#ifndef IRQ_H
#define IRQ_H

#define IRQ0_DE 0
#define IRQ1_DB 1
#define IRQ2_NMI 2
#define IRQ3_BP 3
#define IRQ4_OF 4
#define IRQ5_BR 5
#define IRQ6_UD 6
#define IRQ7_NM 7
#define IRQ8_DF 8
#define IRQ10_TS 10
#define IRQ11_NP 11
#define IRQ12_SS 12
#define IRQ13_GP 13
#define IRQ14_PF 14
#define IRQ16_MF 16
#define IRQ17_AC 17
#define IRQ18_MC 18
#define IRQ19_XM 19
#define IRQ20_VE 20
#define IRQ21_CP 21

#define IRQ0_TIMER 0x20
#define IRQ1_KEYBOARD 0x21

#define PIC0_ICW1 0x20
#define PIC0_ICW2 0x21
#define PIC0_ICW3 0x21
#define PIC0_ICW4 0x21
#define PIC0_IMR 0x21
#define PIC0_OCW2 0x20

#define PIC1_ICW1 0xA0
#define PIC1_ICW2 0xA1
#define PIC1_ICW3 0xA1
#define PIC1_ICW4 0xA1
#define PIC1_IMR 0xA1
#define PIC1_OCW2 0xA0

#define PIC_ICW1_ALWASY_1 (1 << 4)
#define PIC_ICW1_ICW4 (1 << 0)
#define PIC0_ICW4_8086 (1 << 0)

#define PIC_OCW2_EOI (1 << 5)

#define IRQ_PIC_START 0x20

#define IRQ14_HARDDISK_PRIMARY (0x20 + 14)

#define ERR_PAGE_P (1 << 0)
#define ERR_PAGE_WR (1 << 1)
#define ERR_PAGE_US (1 << 1)

#define ERR_EXT (1 << 0)
#define ERR_IDT (1 << 1)

#include "comm/types.h"

typedef struct _exception_frame_t {
    uint32_t gs, fs, es, ds;
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t num, error_code;
    uint32_t eip, cs, eflags;
    uint32_t esp3, ss3;
} exception_frame_t;

/**
 * @brief 定义了一个函数指针类型 irq_handler_t,
 * 指向一个参数为 exception_frame_t* 类型的函数
 * 这意味着该函数类型可以用于处理异常
 * @param {exception_frame_t*} frame exception_frame_t
 * @return {*}
 */
typedef void (*irq_handler_t)(exception_frame_t* frame);

/**
 * @brief 初始化中断向量表
 * @return {void}
 */
void irq_init(void);

void exception_handler_unknown(void);

void exception_handler_divider(void);

void exception_handler_Debug(void);

void exception_handler_NMI(void);

void exception_handler_breakpoint(void);

void exception_handler_overflow(void);

void exception_handler_bound_range(void);

void exception_handler_invalid_opcode(void);

void exception_handler_device_unavailable(void);

void exception_handler_double_fault(void);

void exception_handler_invalid_tss(void);

void exception_handler_segment_not_present(void);

void exception_handler_stack_segment_fault(void);

void exception_handler_general_protection(void);

void exception_handler_page_fault(void);

void exception_handler_fpu_error(void);

void exception_handler_alignment_check(void);

void exception_handler_machine_check(void);

void exception_handler_smd_exception(void);

void exception_handler_virtual_exception(void);

int irq_install(int irq_num, irq_handler_t handler);

void irq_disable(int irq_num);

void irq_enable(int irq_num);

void irq_disable_global(void);

void irq_enable_global(void);

void pic_send_eoi(int irq_num);

typedef uint32_t irq_state_t;

/**
 * @brief 进入保护状态
 * @return {irq_state_t}
 */
irq_state_t irq_enter_protection(void);

/**
 * @brief 退出保护状态
 * @param {irq_state_t} state
 * @return {*}
 */
void irq_leave_protection(irq_state_t state);

#endif
