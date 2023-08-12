/*
 * @Author: warrior
 * @Date: 2023-08-12 21:13:17
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-12 21:57:27
 * @Description:
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"

#define CONSOLE_DISP_ADDR 0xb8000               // 显存的默认起始地址
#define CONSOLE_DISP_END (0xb8000 + 32 * 1024)  // 显存结束地址
#define CONSOLE_ROW_MAX 25                      // 显存区域行数
#define CONSOLE_COL_MAX 80                      // 显存区域列数
#define CONSOLE_NR 1                            // 显存区域列数

/**
 * 描述显示的字符
 */
typedef struct _disp_char_t {
    uint16_t v;
} disp_char_t;

/**
 * 描述一个显示区域
 */
typedef struct _console_t {
    disp_char_t* disp_base;          // 当前显示区域的起始地址
    int display_rows, dispaly_cols;  // 行,列
} console_t;

/**
 * @brief 初始化显示区域
 * @return {*}
 */
int console_init(void);

/**
 * @brief 往显示区域写入
 * @param  dev 往哪个区域写
 * @param  data 写入的数据
 * @param  size 写入的大小
 * @return {*}
 */
int console_write(int dev, char* data, int size);

/**
 * @brief 关闭一个显示区域
 * @param  dev 哪个区域
 * @return {*}
 */
void console_close(int dev);

#endif