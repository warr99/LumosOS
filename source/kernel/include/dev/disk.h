#ifndef DISK_H
#define DISK_H

#include "comm/types.h"

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

#define	DISK_DRIVE_BASE		    0xE0		// 驱动器号基础值:0xA0 + LBA

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
} disk_t;

/**
 * @brief 磁盘初始化
 */
void disk_init(void);

#endif
