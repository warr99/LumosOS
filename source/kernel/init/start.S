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

.macro exception_handler name num  with_err_code
    .text 
    .extern do_handler_\name
    .global exception_handler_\name
exception_handler_\name:

    .if \with_err_code == 0
        push $0
    .endif

    push $\num

    // 现场保护
    pusha
    push %ds
    push %es
    push %fs
    push %gs

    push %esp

    // 中断处理
    call do_handler_\name

    // add $(1*4), %esp -> 一个栈的单元大小为 4 ,将 esp 的值 +4 相当于将栈顶指针往栈底的方向移动一个单元
    pop %esp

    // 恢复
    pop %gs
    pop %fs
    pop %es
    pop %ds
    popa

    add $(2*4), %esp

    // 中断返回指令 
    iret
.endm

exception_handler unknown, -1, 0
exception_handler divider, 0, 0



