/*
 * @Author: warrior
 * @Date: 2023-08-12 21:56:03
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-15 16:46:12
 * @Description:
 */
#include "dev/console.h"
#include "comm/cpu_instr.h"
#include "tools/klib.h"

static console_t console_buf[CONSOLE_NR];

/**
 * @brief 读取当前光标的位置
 */
static int read_cursor_pos(void) {
    int pos;
    outb(0x3D4, 0x0F);  // 写低地址
    pos = inb(0x3D5);
    outb(0x3D4, 0x0E);  // 写高地址
    pos |= inb(0x3D5) << 8;
    return pos;
}

/**
 * @brief 更新光标的位置
 */
static void update_cursor_pos(console_t* console) {
    uint16_t pos = console->cursor_row * console->display_cols + console->cursor_col;

    outb(0x3D4, 0x0F);  // 写低地址
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);  // 写高地址
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

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

static void move_to_col0(console_t* console) {
    console->cursor_col = 0;
}

static void move_next_line(console_t* console) {
    console->cursor_row++;
    if (console->cursor_row >= console->display_rows) {
        scroll_up(console, 1);
    }
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

/**
 * @brief 左移n个字符
 * @param  console 显示区域
 * @param  n n个字符
 * @return 0->成功 -1->失败
 */
static int move_backword(console_t* console, int n) {
    int status = -1;
    for (int i = 0; i < n; i++) {
        if (console->cursor_col > 0) {
            console->cursor_col--;
            status = 0;
        } else if (console->cursor_row > 0) {
            console->cursor_row--;
            console->cursor_col = console->display_cols - 1;
            status = 0;
        }
    }
    return status;
}

static void show_char(console_t* console, char c) {
    int offset = console->cursor_col + console->cursor_row * console->display_cols;
    disp_char_t* p = console->disp_base + offset;
    p->c = c;
    p->foreground = console->foreground;
    p->background = console->background;
    move_forward(console, 1);
}

/**
 * @brief  擦除前一字符
 * @param console 显示区域
 * @return void
 */
static void erase_backword(console_t* console) {
    if (move_backword(console, 1) == 0) {
        show_char(console, ' ');
        move_backword(console, 1);
    }
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

int console_init(int index) {
    console_t* console = console_buf + index;

    if (index = 0) {
        int cursor_pos = read_cursor_pos();
        console->cursor_row = cursor_pos / console->display_cols;
        console->cursor_col = cursor_pos % console->display_cols;
    } else {
        console->cursor_row = 0;
        console->cursor_col = 0;
        clear_display(console);
        update_cursor_pos(console);
    }

    console->display_cols = CONSOLE_COL_MAX;
    console->display_rows = CONSOLE_ROW_MAX;

    console->foreground = COLOR_White;
    console->background = COLOR_Black;
    console->old_cursor_col = console->cursor_col;
    console->old_cursor_row = console->cursor_row;
    console->write_state = CONSOLE_WRITE_ESC;
    console->disp_base = (disp_char_t*)CONSOLE_DISP_ADDR + index * (CONSOLE_COL_MAX * CONSOLE_ROW_MAX);

    return 0;
}

void save_cursor(console_t* console) {
    console->old_cursor_col = console->cursor_col;
    console->old_cursor_row = console->cursor_row;
}

void restore_cursor(console_t* console) {
    console->cursor_col = console->old_cursor_col;
    console->cursor_row = console->old_cursor_row;
}

/**
 * 清空参数表
 */
static void clear_esc_param(console_t* console) {
    kernel_memset(console->esc_param, 0, sizeof(console->esc_param));
    console->curr_param_index = 0;
}

/**
 * 设置字符属性
 */
static void set_font_style(console_t* console) {
    static const cclor_t color_table[] = {
        COLOR_Black, COLOR_Red, COLOR_Green, COLOR_Yellow,   // 0-3
        COLOR_Blue, COLOR_Magenta, COLOR_Cyan, COLOR_White,  // 4-7
    };

    for (int i = 0; i < console->curr_param_index; i++) {
        int param = console->esc_param[i];
        if ((param >= 30) && (param <= 37)) {  // 前景色：30-37
            console->foreground = color_table[param - 30];
        } else if ((param >= 40) && (param <= 47)) {
            console->background = color_table[param - 40];
        } else if (param == 39) {  // 39=默认前景色
            console->foreground = COLOR_White;
        } else if (param == 49) {  // 49=默认背景色
            console->background = COLOR_Black;
        }
    }
}

static void write_normal(console_t* console, char c) {
    switch (c) {
        case ASCII_ESC:
            console->write_state = CONSOLE_WRITE_ESC;
            break;
        case 0x7F:  // 删除
            erase_backword(console);
            break;
        case '\b':  // 左移一个字符
            move_backword(console, 1);
            break;
        case '\r':
            move_to_col0(console);
            break;
        case '\n':
            // 移动到第一列
            move_to_col0(console);
            // 移动到下一行
            move_next_line(console);
            break;
        default: {
            if ((c >= ' ') && (c <= '~')) {
                show_char(console, c);
            }
            break;
        }
    }
}

static void write_esc(console_t* console, char c) {
    switch (c) {
        case '7':  // ESC 7 保存光标
            save_cursor(console);
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
        case '8':  // ESC 8 恢复光标
            restore_cursor(console);
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
        case '[':
            // 清空参数列表
            clear_esc_param(console);
            console->write_state = CONSOLE_WRITE_SQUARE;
            break;
        default:
            console->write_state = CONSOLE_WRITE_NORMAL;
            break;
    }
}

/**
 * @brief 光标左移，但不起始左边界，也不往上移
 */
static void move_left(console_t* console, int n) {
    // 至少移致动1个
    if (n == 0) {
        n = 1;
    }

    int col = console->cursor_col - n;
    console->cursor_col = (col >= 0) ? col : 0;
}

/**
 * @brief 光标右移，但不起始右边界，也不往下移
 */
static void move_right(console_t* console, int n) {
    // 至少移致动1个
    if (n == 0) {
        n = 1;
    }

    int col = console->cursor_col + n;
    if (col >= console->display_cols) {
        console->cursor_col = console->display_cols - 1;
    } else {
        console->cursor_col = col;
    }
}

/**
 * 移动光标
 */
static void move_cursor(console_t* console) {
    console->cursor_row = console->esc_param[0];
    console->cursor_col = console->esc_param[1];
}

/**
 * 擦除字符操作
 */
static void erase_in_display(console_t* console) {
    if (console->curr_param_index < 0) {
        return;
    }

    int param = console->esc_param[0];
    if (param == 2) {
        // 擦除整个屏幕
        erase_rows(console, 0, console->display_rows - 1);
        console->cursor_col = console->cursor_row = 0;
    }
}

static void write_esc_square(console_t* console, char c) {
    if ((c >= '0') && (c <= '9')) {
        // 解析当前参数
        int* param = &console->esc_param[console->curr_param_index];
        *param = *param * 10 + c - '0';
    } else if ((c == ';') && console->curr_param_index < ESC_PARAM_MAX) {
        // 参数结束，继续处理下一个参数
        console->curr_param_index++;
    } else {
        // 结束上一字符的处理
        console->curr_param_index++;

        // 已经接收到所有的字符，继续处理
        switch (c) {
            case 'm':  // 设置字符属性
                set_font_style(console);
                break;
            case 'D':  // 光标左移n个位置 ESC [Pn D
                move_left(console, console->esc_param[0]);
                break;
            case 'C':
                move_right(console, console->esc_param[0]);
                break;
            case 'H':
            case 'f':
                move_cursor(console);
                break;
            case 'J':
                erase_in_display(console);
                break;
            default:
                break;
        }
        console->write_state = CONSOLE_WRITE_NORMAL;
    }
}

int console_write(int dev, char* data, int size) {
    console_t* console = console_buf + dev;
    int len;
    for (len = 0; len < size; len++) {
        char c = *data++;
        switch (console->write_state) {
            case CONSOLE_WRITE_NORMAL:
                write_normal(console, c);
                break;
            case CONSOLE_WRITE_ESC:
                write_esc(console, c);
                break;
            case CONSOLE_WRITE_SQUARE:
                write_esc_square(console, c);
                break;
            default:
                break;
        }
    }
    update_cursor_pos(console);
    return len;
}

void console_close(int dev) {
}
