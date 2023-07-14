/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-13 21:15:39
 * @Description:
 */
#include "init.h"
#include "comm./boot_info.h"
#include "cpu/cpu.h"

void kernel_init(boot_info_t* boot_info) {
    cpu_init();
}

void init_main(void) {
    for(;;){}
}