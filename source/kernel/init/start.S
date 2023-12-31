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
exception_handler Debug, 1, 0
exception_handler NMI, 2, 0
exception_handler breakpoint, 3, 0
exception_handler overflow, 4, 0
exception_handler bound_range, 5, 0
exception_handler invalid_opcode, 6, 0
exception_handler device_unavailable, 7, 0
exception_handler double_fault, 8, 1
exception_handler invalid_tss, 10, 1
exception_handler segment_not_present, 11, 1
exception_handler stack_segment_fault, 12, 1
exception_handler general_protection, 13, 1
exception_handler page_fault, 14, 1
exception_handler fpu_error, 16, 0
exception_handler alignment_check, 17, 1
exception_handler machine_check, 18, 0
exception_handler smd_exception, 19, 0
exception_handler virtual_exception, 20, 0

exception_handler timer, 0x20, 0
exception_handler kbd, 0x21, 0
exception_handler ide_primary, 0x2E, 0


    .text
    .global simple_switch
 simple_switch:
    // 取出源任务的栈顶指针的地址
    mov 4(%esp), %eax
    // 取出后一个任务的栈顶指针的值
    mov 8(%esp), %edx

    // 将需要保存的寄存器入栈
    push %ebp
    push %ebx
    push %esi
    push %edi
    // 保存源任务栈顶的地址保存到 %eax 的中保存的地址
    // 也就是设置 task->stack 的值,注意: %eax 保存的是 task->stack 所在的地址
    mov %esp,(%eax)

    // 将目标任务栈顶的地址给到 %esp (切换到后一个函数的栈)
    mov %edx, %esp
    // 从后一个任务的栈中恢复寄存器的值
    pop %edi
    pop %esi
    pop %ebx
    pop %ebp

    ret

    .global exception_handler_syscall
    .extern do_handler_syscall
exception_handler_syscall:

	pusha
	push %ds
	push %es
	push %fs
	push %gs
	pushf

	mov $(KERNEL_SELECTOR_DS), %eax
	mov %eax, %ds
	mov %eax, %es
	mov %eax, %fs
	mov %eax, %gs

    mov %esp, %eax
    push %eax
	call do_handler_syscall
	add $4, %esp

	popf
	pop %gs
	pop %fs
	pop %es
	pop %ds
	popa

    retf $(5*4)