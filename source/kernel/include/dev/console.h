/*
 * @Author: warrior
 * @Date: 2023-08-12 21:13:17
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-15 17:59:00
 * @Description:
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include "comm/types.h"
#include "dev/tty.h"

#define CONSOLE_DISP_ADDR 0xb8000               // 显存的默认起始地址
#define CONSOLE_DISP_END (0xb8000 + 32 * 1024)  // 显存结束地址
#define CONSOLE_ROW_MAX 25                      // 显存区域行数
#define CONSOLE_COL_MAX 80                      // 显存区域列数
#define CONSOLE_NR 8                            // 显存区域列数

#define ASCII_ESC 0x1b    // ESC 的 ASCII 码
#define ESC_PARAM_MAX 10  // 最多支持的ESC [ 参数数量

// 各种颜色
typedef enum _cclor_t {
    COLOR_Black = 0,
    COLOR_Blue = 1,
    COLOR_Green = 2,
    COLOR_Cyan = 3,
    COLOR_Red = 4,
    COLOR_Magenta = 5,
    COLOR_Brown = 6,
    COLOR_Gray = 7,
    COLOR_Dark_Gray = 8,
    COLOR_Light_Blue = 9,
    COLOR_Light_Green = 10,
    COLOR_Light_Cyan = 11,
    COLOR_Light_Red = 12,
    COLOR_Light_Magenta = 13,
    COLOR_Yellow = 14,
    COLOR_White = 15
} cclor_t;

/**
 * 描述显示的字符
 */
typedef union {
    struct {
        char c;               // 显示的字符
        char foreground : 4;  // 前景色
        char background : 3;  // 背景色
    };

    uint16_t v;
} disp_char_t;

/**
 * 描述一个显示区域
 */
typedef struct _console_t {
    enum {
        CONSOLE_WRITE_NORMAL,  // 普通模式
        CONSOLE_WRITE_ESC,     // ESC转义序列
        CONSOLE_WRITE_SQUARE,  // ESC [接收状态
    } write_state;
    disp_char_t* disp_base;              // 当前显示区域的起始地址
    int display_rows, display_cols;      // 行,列
    int cursor_row, cursor_col;          // 光标所在行,列
    cclor_t foreground, background;      // 前后景色
    int old_cursor_row, old_cursor_col;  // 保存的光标行,列
    int esc_param[ESC_PARAM_MAX];        // 存储参数
    int curr_param_index;                // 当前正在处理保存的参数索引
} console_t;

/**
 * @brief 初始化显示区域
 * @param 显示区域
 * @return {*}
 */
int console_init(int index);

/**
 * @brief 往显示区域写入
 * @param  tty tty设备
 * @return {*}
 */
int console_write(tty_t* tty);

/**
 * @brief 关闭一个显示区域
 * @param  dev 哪个区域
 * @return {*}
 */
void console_close(int dev);

#endif