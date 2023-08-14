/*
 * @Author: warrior
 * @Date: 2023-08-14 10:34:50
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-14 14:10:41
 * @Description: 
 */
#ifndef KBD_H
#define KBD_H

#include "comm/types.h"

#define KBD_PORT_DATA			0x60
#define KBD_PORT_STAT			0x64
#define KBD_PORT_CMD			0x64

#define KBD_STAT_RECV_READY		(1 << 0)
#define KBD_STAT_SEND_FULL		(1 << 1)

#define KEY_RSHIFT		0x36
#define KEY_LSHIFT 		0x2A

/**
 * 键盘扫描码表单元类型
 * 每个按键至多有两个功能键值
 * code1：无shift按下或numlock灯亮的值，即缺省的值
 * code2：shift按下或者number灯灭的值，即附加功能值
 */
typedef struct _key_map_t {
	uint8_t normal;				// 普通功能
	uint8_t func;				// 第二功能
}key_map_t;  

/**
 * 状态指示灯
 */
typedef struct _kbd_state_t {
    int lshift_press : 1;       // 左shift按下
    int rshift_press : 1;       // 右shift按下
}kbd_state_t;

void kbd_init(void);

void exception_handler_kbd (void);

#endif