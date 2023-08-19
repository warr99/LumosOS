/*
 * @Author: warrior
 * @Date: 2023-08-16 14:37:08
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 15:25:03
 * @Description:
 */
#ifndef FILE_H
#define FILE_H

#define FILE_TABLE_SIZE 2048  // 可打开的文件数量
#define FILE_NAME_SIZE 32     // 文件名称大小

#include "comm/types.h"

/**
 * 文件类型
 */
typedef enum _file_type_t {
    FILE_UNKNOWN = 0,
    FILE_TTY = 1,
} file_type_t;

/**
 * 文件描述符
 */
typedef struct _file_t {
    char file_name[FILE_NAME_SIZE];  // 文件名
    file_type_t type;                // 文件类型
    uint32_t size;                   // 文件大小
    int ref;                         // 引用计数

    int dev_id;  // 文件所属的设备号

    int pos;           // 当前位置
    int mode;          // 读写模式
    struct _fs_t* fs;  // 所在的文件系统
} file_t;

/**
 * @brief 分配文件表表项
 * @return file_t*
 */
file_t* file_alloc(void);

/**
 * @brief 释放文件描述符
 * @param {file_t*} file 表项
 */
void file_free(file_t* file);

/**
 * @brief 初始化文件表
 */
void file_table_init(void);

/**
 * @brief 增加file的引用计数
 */
void file_inc_ref(file_t* file);

#endif