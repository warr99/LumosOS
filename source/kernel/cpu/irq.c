/*
 * @Author: warrior
 * @Date: 2023-07-15 09:50:54
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-04 17:09:27
 * @Description:
 */
#include "cpu/irq.h"
#include "comm/cpu_instr.h"
#include "core/syscall.h"
#include "cpu/cpu.h"
#include "os_cfg.h"
#include "tools/log.h"

#define IDT_TABLE_NR 256

void exception_handler_unknown(void);

/**
 * 中断向量表
 */
static gate_desc_t idt_table[IDT_TABLE_NR];

static void dump_core_regs(exception_frame_t* frame) {
    uint32_t ss, esp;
    if (frame->cs & 0x3) {
        ss = frame->ss3;
        esp = frame->esp3;
    } else {
        ss = frame->ds;
        esp = frame->esp;
    }

    log_printf("IRQ: %d, error code: %d.", frame->num, frame->error_code);
    log_printf("CS: %d\n\rDS: %d\n\rES:%d\n\rSS:%d\n\rFS:%d\n\rGS:%d",
               frame->cs, frame->ds, frame->es, ss, frame->fs, frame->gs);
    log_printf(
        "EAX:0x%x\n\r"
        "EBX:0x%x\n\r"
        "ECX:0x%x\n\r"
        "EDX:0x%x\n\r"
        "EDI:0x%x\n\r"
        "ESI:0x%x\n\r"
        "EBP:0x%x\n\r"
        "ESP:0x%x\n\r",
        frame->eax, frame->ebx, frame->ecx, frame->edx,
        frame->edi, frame->esi, frame->ebp, esp);
    log_printf("EIP:0x%x\n\rEFLAGS:0x%x\n\r", frame->eip, frame->eflags);
}

static void init_pic(void) {
    outb(PIC0_ICW1, PIC_ICW1_ALWASY_1 | PIC_ICW1_ICW4);
    outb(PIC0_ICW2, IRQ_PIC_START);
    outb(PIC0_ICW3, 1 << 2);
    outb(PIC0_ICW4, PIC0_ICW4_8086);

    outb(PIC1_ICW1, PIC_ICW1_ALWASY_1 | PIC_ICW1_ICW4);
    outb(PIC1_ICW2, IRQ_PIC_START + 8);
    outb(PIC1_ICW3, 2);
    outb(PIC1_ICW4, PIC0_ICW4_8086);

    outb(PIC0_IMR, 0xFF & ~(1 << 2));
    outb(PIC1_IMR, 0xff);
}

void irq_init(void) {
    for (int i = 0; i < IDT_TABLE_NR; i++) {
        gate_desc_set(idt_table + i, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_unknown, GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);
    }

    irq_install(IRQ0_DE, (irq_handler_t)exception_handler_divider);
    irq_install(IRQ1_DB, (irq_handler_t)exception_handler_Debug);
    irq_install(IRQ2_NMI, (irq_handler_t)exception_handler_NMI);
    irq_install(IRQ3_BP, (irq_handler_t)exception_handler_breakpoint);
    irq_install(IRQ4_OF, (irq_handler_t)exception_handler_overflow);
    irq_install(IRQ5_BR, (irq_handler_t)exception_handler_bound_range);
    irq_install(IRQ6_UD, (irq_handler_t)exception_handler_invalid_opcode);
    irq_install(IRQ7_NM, (irq_handler_t)exception_handler_device_unavailable);
    irq_install(IRQ8_DF, (irq_handler_t)exception_handler_double_fault);
    irq_install(IRQ10_TS, (irq_handler_t)exception_handler_invalid_tss);
    irq_install(IRQ11_NP, (irq_handler_t)exception_handler_segment_not_present);
    irq_install(IRQ12_SS, (irq_handler_t)exception_handler_stack_segment_fault);
    irq_install(IRQ13_GP, (irq_handler_t)exception_handler_general_protection);
    irq_install(IRQ14_PF, (irq_handler_t)exception_handler_page_fault);
    irq_install(IRQ16_MF, (irq_handler_t)exception_handler_fpu_error);
    irq_install(IRQ17_AC, (irq_handler_t)exception_handler_alignment_check);
    irq_install(IRQ18_MC, (irq_handler_t)exception_handler_machine_check);
    irq_install(IRQ19_XM, (irq_handler_t)exception_handler_smd_exception);
    irq_install(IRQ20_VE, (irq_handler_t)exception_handler_virtual_exception);

    irq_install(0x80, (irq_handler_t)exception_handler_syscall);
    gate_desc_set(idt_table + 0x80, KERNEL_SELECTOR_CS, (uint32_t)exception_handler_syscall, GATE_P_PRESENT | GATE_DPL3 | GATE_TYPE_IDT);

    lidt((uint32_t)idt_table, sizeof(idt_table));

    init_pic();
}

int irq_install(int irq_num, irq_handler_t handler) {
    if (irq_num >= IDT_TABLE_NR) {
        return -1;
    }
    gate_desc_set(idt_table + irq_num, KERNEL_SELECTOR_CS, (uint32_t)handler, GATE_P_PRESENT | GATE_DPL0 | GATE_TYPE_IDT);
    return 0;
}

/**
 * @brief
 * @param {exception_frame_t*} frame
 * @param {char*} message 错误信息
 * @return {*}
 */
static void do_default_handler(exception_frame_t* frame, const char* message) {
    log_printf("----------------------------");
    log_printf("IRQ/Exception: %s", message);
    dump_core_regs(frame);
    for (;;) {
        hlt();
    }
}

void do_handler_unknown(exception_frame_t* frame) {
    do_default_handler(frame, "Unknown Exception");
}

void do_handler_divider(exception_frame_t* frame) {
    do_default_handler(frame, "Divider Exception");
}

void do_handler_Debug(exception_frame_t* frame) {
    do_default_handler(frame, "Debug Exception");
}

void do_handler_NMI(exception_frame_t* frame) {
    do_default_handler(frame, "NMI Interrupt.");
}

void do_handler_breakpoint(exception_frame_t* frame) {
    do_default_handler(frame, "Breakpoint.");
}

void do_handler_overflow(exception_frame_t* frame) {
    do_default_handler(frame, "Overflow.");
}

void do_handler_bound_range(exception_frame_t* frame) {
    do_default_handler(frame, "BOUND Range Exceeded.");
}

void do_handler_invalid_opcode(exception_frame_t* frame) {
    do_default_handler(frame, "Invalid Opcode.");
}

void do_handler_device_unavailable(exception_frame_t* frame) {
    do_default_handler(frame, "Device Not Available.");
}

void do_handler_double_fault(exception_frame_t* frame) {
    do_default_handler(frame, "Double Fault.");
}

void do_handler_invalid_tss(exception_frame_t* frame) {
    do_default_handler(frame, "Invalid TSS");
}

void do_handler_segment_not_present(exception_frame_t* frame) {
    do_default_handler(frame, "Segment Not Present.");
}

void do_handler_stack_segment_fault(exception_frame_t* frame) {
    do_default_handler(frame, "Stack-Segment Fault.");
}

void do_handler_general_protection(exception_frame_t* frame) {
    log_printf("--------------------------------");
    log_printf("IRQ/Exception happend: General Protection.");
    if (frame->error_code & ERR_EXT) {
        log_printf(
            "the exception occurred during delivery of an"
            "event external to the program, such as an interrupt"
            "or an earlier exception.");
    } else {
        log_printf(
            "the exception occurred during delivery of a"
            "software interrupt (INT n, INT3, or INTO).");
    }

    if (frame->error_code & ERR_IDT) {
        log_printf(
            "the index portion of the error code refers "
            "to a gate descriptor in the IDT");
    } else {
        log_printf("the index refers to a descriptor in the GDT");
    }

    log_printf("segment index: %d", frame->error_code & 0xFFF8);

    dump_core_regs(frame);
    while (1) {
        hlt();
    }
    // if (frame->cs & 0x3) {
    //     sys_exit(frame->error_code);
    // } else {
    //     for (;;) {
    //         hlt();
    //     }
    // }
}

void do_handler_page_fault(exception_frame_t* frame) {
    log_printf("--------------------------------");
    log_printf("IRQ/Exception happend: Page fault.");
    if (frame->error_code & ERR_PAGE_P) {
        log_printf("\tpage-level protection violation: 0x%x.", read_cr2());
    } else {
        log_printf("\tPage doesn't present 0x%x", read_cr2());
    }

    if (frame->error_code & ERR_PAGE_WR) {
        log_printf("\tThe access causing the fault was a read.");
    } else {
        log_printf("\tThe access causing the fault was a write.");
    }

    if (frame->error_code & ERR_PAGE_US) {
        log_printf("\tA supervisor-mode access caused the fault.");
    } else {
        log_printf("\tA user-mode access caused the fault.");
    }

    dump_core_regs(frame);
    while (1) {
        hlt();
    }

    // if (frame->cs & 0x3) {
    //     sys_exit(frame->error_code);
    // } else {
    //     for (;;) {
    //         hlt();
    //     }
    // }
}

void do_handler_fpu_error(exception_frame_t* frame) {
    do_default_handler(frame, "X87 FPU Floating Point Error.");
}

void do_handler_alignment_check(exception_frame_t* frame) {
    do_default_handler(frame, "Alignment Check.");
}

void do_handler_machine_check(exception_frame_t* frame) {
    do_default_handler(frame, "Machine Check.");
}

void do_handler_smd_exception(exception_frame_t* frame) {
    do_default_handler(frame, "SIMD Floating Point Exception.");
}

void do_handler_virtual_exception(exception_frame_t* frame) {
    do_default_handler(frame, "Virtualization Exception.");
}

void irq_enable(int irq_num) {
    if (irq_num < IRQ_PIC_START) {
        return;
    }

    irq_num -= IRQ_PIC_START;
    if (irq_num < 8) {
        uint8_t mask = inb(PIC0_IMR) & ~(1 << irq_num);
        outb(PIC0_IMR, mask);
    } else {
        irq_num -= 8;
        uint8_t mask = inb(PIC1_IMR) & ~(1 << irq_num);
        outb(PIC1_IMR, mask);
    }
}

void irq_disable(int irq_num) {
    if (irq_num < IRQ_PIC_START) {
        return;
    }

    irq_num -= IRQ_PIC_START;
    if (irq_num < 8) {
        uint8_t mask = inb(PIC0_IMR) | (1 << irq_num);
        outb(PIC0_IMR, mask);
    } else {
        irq_num -= 8;
        uint8_t mask = inb(PIC1_IMR) | (1 << irq_num);
        outb(PIC1_IMR, mask);
    }
}

void irq_disable_global(void) {
    cli();
}

void irq_enable_global(void) {
    sti();
}

void pic_send_eoi(int irq_num) {
    irq_num -= IRQ_PIC_START;

    // 从片也可能需要发送EOI
    if (irq_num >= 8) {
        outb(PIC1_OCW2, PIC_OCW2_EOI);
    }

    outb(PIC0_OCW2, PIC_OCW2_EOI);
}

irq_state_t irq_enter_protection() {
    irq_state_t state = read_eflags();
    irq_disable_global();
    return state;
}

void irq_leave_protection(irq_state_t state) {
    write_eflags(state);
}