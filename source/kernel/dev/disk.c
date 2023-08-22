/*
 * @Author: warrior
 * @Date: 2023-08-19 22:27:07
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-22 12:21:30
 * @Description:
 */
#include "dev/disk.h"
#include "comm/boot_info.h"
#include "comm/cpu_instr.h"
#include "tools/klib.h"
#include "tools/log.h"

static disk_t disk_buf[DISK_CNT];  // 多个磁盘的信息

/**
 * 发送ata命令，支持多达16位的扇区，对我们目前的程序来书够用了。
 */
static void ata_send_cmd(disk_t* disk, uint32_t start_sector, uint32_t sector_count, int cmd) {
    outb(DISK_DRIVE(disk), DISK_DRIVE_BASE | disk->drive);  // 使用LBA寻址，并设置驱动器

    // 必须先写高字节
    outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count >> 8));  // 扇区数高8位
    outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 24));       // LBA参数的24~31位
    outb(DISK_LBA_MID(disk), 0);                                  // 高于32位不支持
    outb(DISK_LBA_HI(disk), 0);                                   // 高于32位不支持
    outb(DISK_SECTOR_COUNT(disk), (uint8_t)(sector_count));       // 扇区数量低8位
    outb(DISK_LBA_LO(disk), (uint8_t)(start_sector >> 0));        // LBA参数的0-7
    outb(DISK_LBA_MID(disk), (uint8_t)(start_sector >> 8));       // LBA参数的8-15位
    outb(DISK_LBA_HI(disk), (uint8_t)(start_sector >> 16));       // LBA参数的16-23位

    // 选择对应的主-从磁盘
    outb(DISK_CMD(disk), (uint8_t)cmd);
}

/**
 * 读取ATA数据端口
 */
static inline void ata_read_data(disk_t* disk, void* buf, int size) {
    uint16_t* c = (uint16_t*)buf;
    for (int i = 0; i < size / 2; i++) {
        *c++ = inw(DISK_DATA(disk));
    }
}

/**
 * 读取ATA数据端口
 */
static inline void ata_write_data(disk_t* disk, void* buf, int size) {
    uint16_t* c = (uint16_t*)buf;
    for (int i = 0; i < size / 2; i++) {
        outw(DISK_DATA(disk), *c++);
    }
}

/**
 * @brief 等待磁盘有数据到达
 */
static inline int ata_wait_data(disk_t* disk) {
    uint8_t status;
    do {
        // 等待数据或者有错误
        status = inb(DISK_STATUS(disk));
        if ((status & (DISK_STATUS_BUSY | DISK_STATUS_DRQ | DISK_STATUS_ERR)) != DISK_STATUS_BUSY) {
            break;
        }
    } while (1);

    // 检查是否有错误
    return (status & DISK_STATUS_ERR) ? -1 : 0;
}

/**
 * @brief 打印磁盘信息
 */
static void print_disk_info(disk_t* disk) {
    log_printf("%s:", disk->name);
    log_printf("  port_base: %x", disk->port_base);
    log_printf("  total_size: %d m", disk->sector_count * disk->sector_size / 1024 / 1024);
    log_printf("  drive: %s", disk->drive == DISK_MASTER ? "Master" : "Slave");
}

/**
 * @brief 检测磁盘相关的信息
 */
static int identify_disk(disk_t* disk) {
    ata_send_cmd(disk, 0, 0, DISK_CMD_IDENTIFY);
    int err = inb(DISK_STATUS(disk));
    if (err == 0) {
        log_printf("%s doesn't exist\n", disk->name);
        return -1;
    }
    err = ata_wait_data(disk);
    if (err < 0) {
        log_printf("disk[%s]: read failed!\n", disk->name);
        return err;
    }
    uint16_t buf[256];
    ata_read_data(disk, buf, sizeof(buf));
    disk->sector_count = *(uint32_t*)(buf + 100);
    disk->sector_size = SECTOR_SIZE;  // 固定为512字节大小
    return 0;
}

void disk_init(void) {
    log_printf("Checking disk...");
    kernel_memset(disk_buf, 0, sizeof(disk_buf));
    for (int i = 0; i < DISK_PER_CHANNEL; i++) {
        disk_t* disk = disk_buf + i;
        // 磁盘命名:sda sdb sdc ...
        kernel_sprintf(disk->name, "sd%c", i + 'a');
        // 磁盘类型
        disk->drive = (i == 0) ? DISK_MASTER : DISK_SLAVE;
        disk->port_base = IOBASE_PRIMARY;
        int err = identify_disk(disk);
        if (err == 0) {
            print_disk_info(disk);
        }
    }
}