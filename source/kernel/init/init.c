/*
 * @Author: warrior
 * @Date: 2023-07-12 19:56:55
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-13 14:07:33
 * @Description:
 */
#include "init.h"
#include "comm./boot_info.h"
#include "cpu/cpu.h"

void kernel_init(boot_info_t* boot_info) {
    cpu_init();
}