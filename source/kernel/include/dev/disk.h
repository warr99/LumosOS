/*
 * @Author: warrior
 * @Date: 2023-08-19 22:13:16
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-22 13:27:58
 * @Description:
 */
#ifndef DISK_H
#define DISK_H

#include "comm/types.h"
#include "ipc/mutex.h"
#include "ipc/sem.h"

#define DISK_NAME_SIZE 32              // 磁盘名称大小
#define PART_NAME_SIZE 32              // 分区名称大小
#define DISK_CNT 2                     // 磁盘的数量
#define DISK_PRIMARY_PART_CNT (4 + 1)  // 主分区数量+拓展分区数量
#define DISK_PER_CHANNEL 2             // 每通道磁盘数量

#define IOBASE_PRIMARY 0x1F0                           // 主总线的基地址
#define DISK_DATA(disk) (disk->port_base + 0)          // 数据寄存器
#define DISK_ERROR(disk) (disk->port_base + 1)         // 错误寄存器
#define DISK_SECTOR_COUNT(disk) (disk->port_base + 2)  // 扇区数量寄存器
#define DISK_LBA_LO(disk) (disk->port_base + 3)        // LBA寄存器
#define DISK_LBA_MID(disk) (disk->port_base + 4)       // LBA寄存器
#define DISK_LBA_HI(disk) (disk->port_base + 5)        // LBA寄存器
#define DISK_DRIVE(disk) (disk->port_base + 6)         // 磁盘或磁头 Drive
#define DISK_STATUS(disk) (disk->port_base + 7)        // 状态寄存器
#define DISK_CMD(disk) (disk->port_base + 7)           // 命令寄存器

// ATA命令
#define DISK_CMD_IDENTIFY 0xEC  // IDENTIFY命令
#define DISK_CMD_READ 0x24      // 读命令
#define DISK_CMD_WRITE 0x34     // 写命令

// 状态寄存器
#define DISK_STATUS_ERR (1 << 0)   // 发生了错误
#define DISK_STATUS_DRQ (1 << 3)   // 准备好接受数据或者输出数据
#define DISK_STATUS_DF (1 << 5)    // 驱动错误
#define DISK_STATUS_BUSY (1 << 7)  // 正忙

#define DISK_DRIVE_BASE 0xE0  // 驱动器号基础值:0xA0 + LBA

#define MBR_PRIMARY_PART_NR 4  // 4个分区表

/**
 * @brief 分区结构
 */
typedef struct _partinfo_t {
    char name[PART_NAME_SIZE];  // 分区名称
    struct _disk_t* disk;       // 所属的磁盘
    int start_sector;           // 起始扇区
    int total_sector;           // 总扇区
    enum {
        FS_INVALID = 0x00,  // 无效文件系统类型
        FS_FAT16_0 = 0x06,  // FAT16 文件系统的类型 0
        FS_FAT16_1 = 0x0E,  // FAT16 文件系统的类型 1
    } type;
} partinfo_t;  // 分区的文件系统类型

/**
 * @brief 磁盘结构
 */
typedef struct _disk_t {
    char name[DISK_NAME_SIZE];                   // 磁盘名称
    int sector_size;                             // 扇区大小
    int sector_count;                            // 扇区数量
    partinfo_t partinfo[DISK_PRIMARY_PART_CNT];  // 磁盘分区表(主分区+拓展分区)
    enum {
        DISK_MASTER = (0 << 4),  // 主设备
        DISK_SLAVE = (1 << 4),   // 从设备
    } drive;                     // 磁盘的类型
    uint16_t port_base;          // 端口起始地址
    mutex_t* mutex;
    sem_t* op_sem;
} disk_t;

#pragma pack(1)
/**
 * MBR的分区表项类型
 */
typedef struct _part_item_t {
    uint8_t boot_active;           // 分区是否活动
    uint8_t start_header;          // 起始header
    uint16_t start_sector : 6;     // 起始扇区
    uint16_t start_cylinder : 10;  // 起始磁道
    uint8_t system_id;             // 文件系统类型
    uint8_t end_header;            // 结束header
    uint16_t end_sector : 6;       // 结束扇区
    uint16_t end_cylinder : 10;    // 结束磁道
    uint32_t relative_sectors;     // 相对于该驱动器开始的相对扇区数
    uint32_t total_sectors;        // 总的扇区数
} part_item_t;

/**
 * MBR区域描述结构
 */
typedef struct _mbr_t {
    uint8_t code[446];  // 引导代码区
    part_item_t part_item[MBR_PRIMARY_PART_NR];
    uint8_t boot_sig[2];  // 引导标志
} mbr_t;
#pragma pack()
/**
 * @brief 磁盘初始化
 */
void disk_init(void);

void exception_handler_ide_primary(void);
#endif
