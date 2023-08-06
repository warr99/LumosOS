/*
 * @Author: warrior
 * @Date: 2023-07-18 10:36:04
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-06 09:46:09
 * @Description:
 */
#include "core/task.h"
#include "comm/cpu_instr.h"
#include "core/memory.h"
#include "core/syscall.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "cpu/mmu.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/log.h"

static uint32_t idle_task_stack[1024];
static task_manager_t task_manager;
static task_t task_table[TASK_NR];
static mutex_t task_table_mutex;

static void idle_task_entry(void) {
    for (;;) {
        hlt();
    }
}

static int tss_init(task_t* task, uint32_t entry, uint32_t esp, int flag) {
    int tss_sel = gdt_alloc_desc();
    if (tss_sel < 0) {
        log_printf("alloc tss failed.\n");
        return -1;
    }
    segment_desc_set(tss_sel, (uint32_t)&task->tss, sizeof(tss_t),
                     SEG_P_PRESENT | SEG_DPL0 | SEG_TYPE_TSS);
    kernel_memset(&task->tss, 0, sizeof(tss_t));

    uint32_t kernel_stack = memory_alloc_one_page();
    if (kernel_stack == 0) {
        goto tss_init_failed;
    }

    int code_sel, data_sel;
    if (flag & TASK_FLAGS_SYSTEM) {
        code_sel = KERNEL_SELECTOR_CS;
        data_sel = KERNEL_SELECTOR_DS;
    } else {
        code_sel = task_manager.app_code_sel | SEG_CPL3;
        data_sel = task_manager.app_data_sel | SEG_CPL3;
    }
    task->tss.eip = entry;
    task->tss.esp = esp;
    task->tss.esp0 = kernel_stack + MEM_PAGE_SIZE;
    task->tss.ss = data_sel;
    task->tss.ss0 = KERNEL_SELECTOR_DS;
    task->tss.es = task->tss.ds = task->tss.fs = task->tss.gs = data_sel;
    task->tss.cs = code_sel;
    task->tss.eflags = EFLAGS_IF | EFLAGS_DEFAULT;
    uint32_t page_dir = memory_create_uvm();
    if (page_dir == 0) {
        goto tss_init_failed;
    }
    task->tss.cr3 = page_dir;
    task->tss_sel = tss_sel;
    return 0;
tss_init_failed:
    gdt_free_sel(tss_sel);
    if (kernel_stack) {
        memory_free_one_page(kernel_stack);
    }
    return -1;
}

/**
 * @brief 分配task结构
 * @return {task_t}
 */
static task_t* alloc_task(void) {
    task_t* task = (task_t*)0;

    mutex_lock(&task_table_mutex);
    for (int i = 0; i < TASK_NR; i++) {
        task_t* curr = task_table + i;
        if (curr->name[0] == 0) {
            task = curr;
            break;
        }
    }
    mutex_unlock(&task_table_mutex);

    return task;
}

/**
 * @brief
 * @param {task_t*} task
 * @return {*}
 */
static void free_task(task_t* task) {
    mutex_lock(&task_table_mutex);
    task->name[0] = 0;
    mutex_unlock(&task_table_mutex);
}

int task_init(task_t* task, const char* name, uint32_t entry, uint32_t esp, int flag) {
    ASSERT(task != (task_t*)0);
    tss_init(task, entry, esp, flag);
    kernel_strncpy(task->name, name, TASK_NAME_SIZE);
    task->state = TASK_CREATED;
    task->time_ticks = TASK_TIME_SLICE_DEFAULT;
    task->slice_ticks = task->time_ticks;
    task->parent = (task_t*)0;
    task->sleep_ticks = 0;
    list_node_init(&task->all_node);
    list_node_init(&task->run_node);
    list_node_init(&task->wait_node);
    irq_state_t state = irq_enter_protection();
    task->pid = (uint32_t)task;
    task_set_ready(task);
    list_insert_last(&task_manager.task_list, &task->all_node);
    irq_leave_protection(state);
    return 0;
}

void task_uninit(task_t* task) {
    if (task->tss_sel) {
        gdt_free_sel(task->tss_sel);
    }
    if (task->tss.esp0) {
        memory_free_one_page(task->tss.esp0 - MEM_PAGE_SIZE);
    }
    if (task->tss.cr3) {
        memory_destroy_uvm(task->tss.cr3);
    }
    kernel_memset(task, 0, sizeof(task_t));
}

/**
 * @brief 任务切换
 * @param {uint32_t**} from 源任务的任务控制块中保存栈顶指针的地址,用于将源任务的栈顶指针保存到该地址中
 * @param {uint32_t*} to 目标任务的任务控制块中保存栈顶指针的值,用于跳转到目标任务的栈顶
 * @return {*}
 */
void simple_switch(uint32_t** from, uint32_t* to);

void task_manager_init(void) {
    kernel_memset(task_table, 0, sizeof(task_table));
    mutex_init(&task_table_mutex);
    // 为应用程序分配数据段选择子和代码段选择子(与内核区分开,特权级为3)
    int sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL | SEG_TYPE_DATA | SEG_TYPE_RW | SEG_D);
    task_manager.app_data_sel = sel;

    sel = gdt_alloc_desc();
    segment_desc_set(sel, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL3 | SEG_S_NORMAL | SEG_TYPE_CODE | SEG_TYPE_RW | SEG_D);
    task_manager.app_code_sel = sel;

    list_init(&task_manager.ready_list);
    list_init(&task_manager.task_list);
    list_init(&task_manager.sleep_list);
    task_manager.curr_task = (task_t*)0;

    task_init(&task_manager.idle_task, "idle_task", (uint32_t)idle_task_entry, (uint32_t)&idle_task_stack[1024], TASK_FLAGS_SYSTEM);
}

task_t* get_first_task(void) {
    return &task_manager.first_task;
}

void task_first_init(void) {
    void first_task_enrty(void);

    extern uint8_t s_first_task[], e_first_task[];
    uint32_t copy_size = (uint32_t)(e_first_task - s_first_task);
    uint32_t alloc_size = 10 * MEM_PAGE_SIZE;
    ASSERT(copy_size < alloc_size);

    uint32_t first_start = (uint32_t)first_task_enrty;

    task_init(&task_manager.first_task, "first task", first_start, first_start + alloc_size, 0);
    write_tr(task_manager.first_task.tss_sel);
    task_manager.curr_task = &task_manager.first_task;
    // 更新CR3寄存器的内容，以切换到新的任务的页表，从而实现不同任务间的地址隔离和内存保护
    mmu_set_page_dir(task_manager.first_task.tss.cr3);

    memory_alloc_page_for(first_start, alloc_size, PTE_P | PTE_W | PTE_U);
    kernel_memcpy((void*)first_start, s_first_task, copy_size);
}

void task_switch(task_t* from, task_t* to) {
    switch_to_tss(to->tss_sel);
    //  simple_switch(&from->stack, to->stack);
}

void task_set_ready(task_t* task) {
    if (task == &task_manager.idle_task) {
        return;
    }

    list_insert_last(&task_manager.ready_list, &task->run_node);
    task->state = TASK_READY;
}

void task_set_block(task_t* task) {
    if (task == &task_manager.idle_task) {
        return;
    }
    list_remove(&task_manager.ready_list, &task->run_node);
}

task_t* task_current(void) {
    return task_manager.curr_task;
}

int sys_sched_yield(void) {
    irq_state_t state = irq_enter_protection();
    if (list_count(&task_manager.ready_list) > 1) {
        task_t* curr_task = task_current();
        task_set_block(curr_task);
        task_set_ready(curr_task);

        task_dispatch();
    }
    irq_leave_protection(state);
    return 0;
}

task_t* task_next_run(void) {
    if (list_count(&task_manager.ready_list) == 0) {
        return &task_manager.idle_task;
    }
    list_node_t* task_node = list_first(&task_manager.ready_list);
    return list_node_parent(task_node, task_t, run_node);
}

void task_dispatch(void) {
    irq_state_t state = irq_enter_protection();
    task_t* to = task_next_run();
    if (to != task_manager.curr_task) {
        task_t* from = task_current();
        task_manager.curr_task = to;
        to->state = TASK_RUNNING;
        task_switch(from, to);
    }
    irq_leave_protection(state);
}

void task_time_tick(void) {
    task_t* curr_task = task_current();
    if (--curr_task->slice_ticks == 0) {
        curr_task->slice_ticks = curr_task->time_ticks;
        task_set_block(curr_task);
        task_set_ready(curr_task);
        task_dispatch();
    }
    list_node_t* curr = list_first(&task_manager.sleep_list);
    while (curr) {
        list_node_t* next = list_node_next(curr);
        task_t* task = list_node_parent(curr, task_t, run_node);
        if (--task->sleep_ticks == 0) {
            task_set_wakeup(task);
            task_set_ready(task);
        }
        curr = next;
    }
    task_dispatch();
}

void task_set_sleep(task_t* task, uint32_t ticks) {
    if (ticks == 0) {
        return;
    }
    task->sleep_ticks = ticks;
    task->state = TASK_SLEEP;
    list_insert_last(&task_manager.sleep_list, &task->run_node);
}

void task_set_wakeup(task_t* task) {
    list_remove(&task_manager.sleep_list, &task->run_node);
}

void sys_sleep(uint32_t ms) {
    irq_state_t state = irq_enter_protection();
    task_set_block(task_manager.curr_task);
    task_set_sleep(task_manager.curr_task, (ms + (OS_TICKS_MS - 1)) / OS_TICKS_MS);
    task_dispatch();
    irq_leave_protection(state);
}

int sys_getpid(void) {
    task_t* task = task_current();
    return task->pid;
}

void sys_print_msg(char* fmt, int arg) {
    log_printf(fmt, arg);
}

/**
 * @brief 创建进程的副本
 */
int sys_fork(void) {
    task_t* parent_task = task_current();

    // 分配任务结构
    task_t* child_task = alloc_task();
    if (child_task == (task_t*)0) {
        goto fork_failed;
    }

    syscall_frame_t* frame = (syscall_frame_t*)(parent_task->tss.esp0 - sizeof(syscall_frame_t));

    // 对子进程进行初始化，并对必要的字段进行调整
    // 其中esp要减去系统调用的总参数字节大小，因为其是通过正常的ret返回, 而没有走系统调用处理的ret(参数个数返回)
    int err = task_init(child_task, parent_task->name, frame->eip, frame->esp + sizeof(uint32_t) * SYSCALL_PARAM_COUNT, 0);
    if (err < 0) {
        goto fork_failed;
    }
    tss_t* tss = &child_task->tss;
    tss->eax = 0;
    tss->ebx = frame->ebx;
    tss->ecx = frame->ecx;
    tss->edx = frame->edx;
    tss->esi = frame->esi;
    tss->edi = frame->edi;
    tss->ebp = frame->ebp;

    tss->cs = frame->cs;
    tss->ds = frame->ds;
    tss->es = frame->es;
    tss->fs = frame->fs;
    tss->gs = frame->gs;
    tss->eflags = frame->eflags;

    child_task->parent = parent_task;

    if ((tss->cr3 = memory_copy_uvm(parent_task->tss.cr3)) < 0) {
        goto fork_failed;
    }
    // tss->cr3 = parent_task->tss.cr3;

    return child_task->pid;

fork_failed:
    if (child_task) {
        task_uninit(child_task);
        free_task(child_task);
    }

    return -1;
}