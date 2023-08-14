/*
 * @Author: warrior
 * @Date: 2023-08-14 10:34:50
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-14 16:50:21
 * @Description:
 */
#ifndef KBD_H
#define KBD_H

#include "comm/types.h"

#define KBD_PORT_DATA 0x60
#define KBD_PORT_STAT 0x64
#define KBD_PORT_CMD 0x64

#define KBD_STAT_RECV_READY (1 << 0)
#define KBD_STAT_SEND_FULL (1 << 1)

#define KEY_RSHIFT 0x36
#define KEY_LSHIFT 0x2A
#define KEY_CAPS 0x3A

#define KEY_CTRL 		0x1D		// E0, 1D或1d
#define KEY_RSHIFT		0x36
#define KEY_LSHIFT 		0x2A
#define KEY_ALT 		0x38		// E0, 38或38

#define	KEY_FUNC		 0x8000
#define KEY_F1			(0x3B)
#define KEY_F2			(0x3C)
#define KEY_F3			(0x3D)
#define KEY_F4			(0x3E)
#define KEY_F5			(0x3F)
#define KEY_F6			(0x40)
#define KEY_F7			(0x41)
#define KEY_F8			(0x42)
#define KEY_F9			(0x43)
#define KEY_F10			(0x44)
#define KEY_F11			(0x57)
#define KEY_F12			(0x58)

#define KEY_E0 0xE0  // E0编码
#define KEY_E1 0xE1  // E1编码

/**
 * 键盘扫描码表单元类型
 * 每个按键至多有两个功能键值
 * code1：无shift按下或numlock灯亮的值，即缺省的值
 * code2：shift按下或者number灯灭的值，即附加功能值
 */
typedef struct _key_map_t {
    uint8_t normal;  // 普通功能
    uint8_t func;    // 第二功能
} key_map_t;

/**
 * 状态指示灯
 */
typedef struct _kbd_state_t {
    int caps_lock : 1;
    int lshift_press : 1;  // 左shift按下
    int rshift_press : 1;  // 右shift按下
    int lctrl_press : 1;
    int rctrl_press : 1;
    int lalt_press : 1;
    int ralt_press : 1;
} kbd_state_t;

void kbd_init(void);

void exception_handler_kbd(void);

#endif