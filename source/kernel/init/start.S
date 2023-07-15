 #include "os_cfg.h"
    .text
    .extern kernel_init
    .extern init_main
    .global _start
_start:
    push %ebp
    mov %esp, %ebp
    mov 0x8(%ebp), %eax
    push %eax

    call kernel_init

    jmp $KERNEL_SELECTOR_CS, $gdt_reload
    
gdt_reload:
	mov	$KERNEL_SELECTOR_DS ,%ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs
    
    mov $(stack + KERNEL_STACK_SIZE), %esp
    jmp init_main

    .bss
.comm stack, KERNEL_STACK_SIZE

    .text
    .extern do_handler_unknown
    .global exception_handler_unknown
exception_handler_unknown:
    // 现场保护
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    // 中断处理
    call do_handler_unknown

    // 恢复
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    // 中断返回指令 
    iret
