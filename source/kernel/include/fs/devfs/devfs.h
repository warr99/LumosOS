/*
 * @Author: warrior
 * @Date: 2023-08-19 10:33:14
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 10:45:12
 * @Description:
 */
#ifndef DEVFS_H
#define DEVFS_H

#include "fs/fs.h"

/**
 * @brief 设备类型描述结构
 */
typedef struct _devfs_type_t {
    const char* name;  // 设备名称(tty disk ...)
    int dev_type;      // 设备类型
    int file_type;     // 对应的文件类型
} devfs_type_t;

#endif