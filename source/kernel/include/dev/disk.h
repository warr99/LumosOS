#ifndef DISK_H
#define DISK_H

#include "comm/types.h"

#define DISK_NAME_SIZE 32              // 磁盘名称大小
#define PART_NAME_SIZE 32              // 分区名称大小
#define DISK_CNT 2                     // 磁盘的数量
#define DISK_PRIMARY_PART_CNT (4 + 1)  // 主分区数量+拓展分区数量

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
} disk_t;

/**
 * @brief 磁盘初始化
 */
void disk_init (void);

#endif
