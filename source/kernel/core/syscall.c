#include "core/syscall.h"
#include "core/task.h"
#include "cpu/irq.h"
#include "tools/log.h"

typedef int (*syscall_handler_t)(uint32_t arg0, uint32_t arg1, uint32_t arg2, uint32_t arg3);

static const syscall_handler_t sys_table[] = {
    [SYS_sleep] = (syscall_handler_t)sys_sleep,
    [SYS_getpid] = (syscall_handler_t)sys_getpid,
    [SYS_printmsg] = (syscall_handler_t)sys_print_msg,
};

void do_handler_syscall(exception_frame_t* frame) {
    int func_id = frame->eax;
    int arg0 = frame->ebx;
    int arg1 = frame->ecx;
    int arg2 = frame->edx;
    int arg3 = frame->esi;
    if (func_id < sizeof(sys_table) / sizeof(sys_table[0])) {
        syscall_handler_t handler = sys_table[func_id];
        if (handler) {
            int ret = handler(arg0, arg1, arg2, arg3);
            // 将栈中的 eax 值进行修改,这样一来,当 start.S 执行 popa 指令时,新的 eax 值就被弹出到 eax 寄存器中,实现函数返回值
            frame->eax = ret;
            return;
        }
    }

    task_t* task = task_current();
    log_printf("task: %s, Unknown syscall: %d", task->name, func_id);
    frame->eax = -1;
}