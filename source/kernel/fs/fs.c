/*
 * @Author: warrior
 * @Date: 2023-08-07 16:42:16
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-16 10:36:33
 * @Description:
 */
#include "fs/fs.h"
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "core/task.h"
#include "dev/console.h"
#include "tools/klib.h"
#include "tools/log.h"

#define TEMP_FILE_ID 100
#define TEMP_ADDR (8 * 1024 * 1024)  // 在0x800000处缓存原始

static uint8_t* temp_pos;  // 当前位置

/**
 * 使用LBA48位模式读取磁盘
 */
static void read_disk(int sector, int sector_count, uint8_t* buf) {
    outb(0x1F6, (uint8_t)(0xE0));

    outb(0x1F2, (uint8_t)(sector_count >> 8));
    outb(0x1F3, (uint8_t)(sector >> 24));  // LBA参数的24~31位
    outb(0x1F4, (uint8_t)(0));             // LBA参数的32~39位
    outb(0x1F5, (uint8_t)(0));             // LBA参数的40~47位

    outb(0x1F2, (uint8_t)(sector_count));
    outb(0x1F3, (uint8_t)(sector));        // LBA参数的0~7位
    outb(0x1F4, (uint8_t)(sector >> 8));   // LBA参数的8~15位
    outb(0x1F5, (uint8_t)(sector >> 16));  // LBA参数的16~23位

    outb(0x1F7, (uint8_t)0x24);

    // 读取数据
    uint16_t* data_buf = (uint16_t*)buf;
    while (sector_count-- > 0) {
        // 每次扇区读之前都要检查，等待数据就绪
        while ((inb(0x1F7) & 0x88) != 0x8) {
        }

        // 读取并将数据写入到缓存中
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1F0);
        }
    }
}

/**
 * 打开文件
 */
int sys_open(const char* name, int flags, ...) {
    if (name[0] == '/') {
        read_disk(5000, 80, (uint8_t*)TEMP_ADDR);
        temp_pos = (uint8_t*)TEMP_ADDR;
        return TEMP_FILE_ID;
    }

    return -1;
}

/**
 * 读取文件api
 */
int sys_read(int file, char* ptr, int len) {
    if (file == TEMP_FILE_ID) {
        kernel_memcpy(ptr, temp_pos, len);
        temp_pos += len;
        return len;
    }
    return -1;
}

/**
 * 写文件
 */
int sys_write(int file, char* ptr, int len) {
    if (file == 1) {
        ptr[len] = '\0';
        log_printf("%s", ptr);
        // console_write(0, ptr, len);
        return len;
    }

    return -1;
}

/**
 * 文件访问位置定位
 */
int sys_lseek(int file, int ptr, int dir) {
    if (file == TEMP_FILE_ID) {
        temp_pos = (uint8_t*)(ptr + TEMP_ADDR);
        return 0;
    } else if (file == 0) {
    }

    return -1;
}

/**
 * 关闭文件
 */
int sys_close(int file) {
}

int sys_isatty(int file) {
    return -1;
}

int sys_fstat(int file, struct stat* st) {
    return -1;
}