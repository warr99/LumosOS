/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-19 15:50:26
 * @Description:
 */

#include "init.h"
#include "comm./boot_info.h"
#include "comm/cpu_instr.h"
#include "core/task.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "dev/time.h"
#include "os_cfg.h"
#include "tools/klib.h"
#include "tools/list.h"
#include "tools/log.h"

void kernel_init(boot_info_t* boot_info) {
    ASSERT(boot_info->ram_region_count != 0);
    cpu_init();
    log_init();
    irq_init();
    time_init();
}

static task_t first_task;
static task_t init_task;
static uint32_t init_task_stack[1024];

void init_task_entry(void) {
    int count = 0;
    for (;;) {
        log_printf("init task: %d", count++);
        task_switch(&init_task, &first_task);
    }
}

void list_test(void) {
    list_t list;
    list_node_t nodes[5];
    list_init(&list);
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("insert first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_first(&list, node);
    }
    log_printf("list:first=0x%x,last=0x%x,count=%d",
               list_first(&list), list_last(&list), list_count(&list));
    for (int i = 0; i < 5; i++) {
        list_node_t* node = list_remove_first(&list);
        log_printf("remove first to list: %d, 0x%x", i, (uint32_t)node);
    }
    log_printf("list:first=0x%x,last=0x%x,count=%d",
               list_first(&list), list_last(&list), list_count(&list));
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        log_printf("insert first to list: %d, 0x%x", i, (uint32_t)node);
        list_insert_first(&list, node);
    }
    log_printf("list:first=0x%x,last=0x%x,count=%d",
               list_first(&list), list_last(&list), list_count(&list));
    for (int i = 0; i < 5; i++) {
        list_node_t* node = nodes + i;
        list_remove(&list, node);
        log_printf("remove first to list: %d, 0x%x", i, (uint32_t)node);
    }
    log_printf("list:first=0x%x,last=0x%x,count=%d",
               list_first(&list), list_last(&list), list_count(&list));

    struct type_t {
        int i;
        list_node_t node;
    } v = {0x123456};

    list_node_t* v_node = &v.node;
    struct type_t* p = list_node_parent(v_node, struct type_t, node);
    if ((p->i == 0x123456)) {
        log_printf("YES!");
    }
}

void init_main(void) {
    list_test();
    log_printf("Kernel is running ...");

    task_init(&init_task, (uint32_t)init_task_entry, (uint32_t)&init_task_stack[1024]);
    task_init(&first_task, (uint32_t)0, 0);
    write_tr(first_task.tss_sel);

    int count = 0;
    for (;;) {
        log_printf("init main: %d", count++);
        task_switch(&first_task, &init_task);
    }

    init_task_entry();
}