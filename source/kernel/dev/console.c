/*
 * @Author: warrior
 * @Date: 2023-08-12 21:56:03
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-13 10:37:10
 * @Description:
 */
#include "dev/console.h"
#include "tools/klib.h"

static console_t console_buf[CONSOLE_NR];

/**
 * @brief 清空某些行
 * @param  console 显示区域
 * @param  start 起始行
 * @param  end 结束行
 * @return
 */
static void erase_rows(console_t* console, int start, int end) {
    volatile disp_char_t* disp_start = console->disp_base + console->display_cols * start;
    volatile disp_char_t* disp_end = console->disp_base + console->display_cols * (end + 1);

    while (disp_start < disp_end) {
        disp_start->c = ' ';
        disp_start->foreground = console->foreground;
        disp_start->background = console->background;

        disp_start++;
    }
}

/**
 * @brief 将显示区域整体上移 line 行
 * @param  console 显示区域
 * @param  line 上移的行数
 * @return
 */
static void scroll_up(console_t* console, int lines) {
    // 目的地
    disp_char_t* dest = console->disp_base;
    // 源地址
    disp_char_t* src = console->disp_base + console->display_cols * lines;
    // 需要拷贝的字节量
    uint32_t size = (console->display_rows - lines) * console->display_cols * sizeof(disp_char_t);
    // 拷贝
    kernel_memcpy(dest, src, size);
    // 清空下面 line 行
    erase_rows(console, console->display_rows - lines, console->display_rows - 1);
    console->cursor_row -= lines;
}

/**
 * @brief 将光标往前移n个字符
 * @param  console 显示区域
 * @param  n 移动多少
 * @return void
 */
static void move_forward(console_t* console, int n) {
    for (int i = 0; i < n; i++) {
        console->cursor_col++;
        if (console->cursor_col >= console->display_cols) {
            console->cursor_col = 0;
            console->cursor_row++;

            if (console->cursor_row >= console->display_cols) {
                scroll_up(console, 1);
            }
        }
    }
}

static void show_char(console_t* console, char c) {
    int offset = console->cursor_col + console->cursor_row * console->display_cols;
    disp_char_t* p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_forward(console, 1);
}

static void clear_display(console_t* console) {
    int size = console->display_cols * console->display_rows;
    disp_char_t* start = console->disp_base;
    for (int i = 0; i < size; i++, start++) {
        start->c = ' ';
        start->foreground = console->foreground;
        start->background = console->background;
    }
}

int console_init(void) {
    for (int i = 0; i < CONSOLE_NR; i++) {
        console_t* console = console_buf + i;

        console->display_cols = CONSOLE_COL_MAX;
        console->display_rows = CONSOLE_ROW_MAX;
        console->cursor_col = 0;
        console->cursor_row = 0;
        console->foreground = COLOR_White;
        console->background = COLOR_Black;
        console->disp_base = (disp_char_t*)CONSOLE_DISP_ADDR + i * (CONSOLE_COL_MAX * CONSOLE_ROW_MAX);

        clear_display(console);
    }
    return 0;
}

static void move_to_col0(console_t* console) {
    console->cursor_col = 0;
}

static void move_next_line(console_t* console) {
    console->cursor_row++;
    if (console->cursor_row >= console->display_rows) {
        scroll_up(console, 1);
    }
}

int console_write(int dev, char* data, int size) {
    console_t* console = console_buf + dev;
    int len;
    for (len = 0; len < size; len++) {
        char c = *data++;
        switch (c) {
            case '\n':
                // 移动到第一列
                move_to_col0(console);
                // 移动到下一行
                move_next_line(console);
                break;
            default:
                show_char(console, c);
                break;
        }
    }
    return len;
}

void console_close(int dev) {
}
