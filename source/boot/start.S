/*
 *                        _oo0oo_
 *                       o8888888o
 *                       88" . "88
 *                       (| -_- |)
 *                       0\  =  /0
 *                     ___/`---'\___
 *                   .' \\|     |// '.
 *                  / \\|||  :  |||// \
 *                 / _||||| -:- |||||- \
 *                |   | \\\  - /// |   |
 *                | \_|  ''\---/''  |_/ |
 *                \  .-\__  '-'  ___/-. /
 *              ___'. .'  /--.--\  `. .'___
 *           ."" '<  `.___\_<|>_/___.' >' "".
 *          | | :  `- \`.;`\ _ /`;.`/ - ` : | |
 *          \  \ `_.   \_ __\ /__ _/   .-` /  /
 *      =====`-.____`.___ \_____/___.-`___.-'=====
 *                        `=---='
 * 
 * 
 *      ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *            佛祖保佑     永不宕机     永无BUG
 * 
 * @Author: warrior
 * @Date: 2023-07-10 16:19:00
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-17 11:37:20
 * @Description: 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 */
	#include "boot.h"

  	// 16位代码,务必加上
  	.code16
 	.text
	.global _start
	.extern boot_entry
_start:
    // 采用平坦模式,所有的段寄存器全部指向0
 	mov $0, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs

	// ESP寄存器指向栈的顶部,设置其指向 0x7c00
	mov $_start, %esp

	// 使用软中断显示'L'字符
	mov $0x13, %ah
	mov $'L', %al
	int $0x10

read_loader:
	mov $0x8000, %bx
	mov $0x2, %ah
	mov $0x2, %cx
	mov $64, %al
	mov $0x0080, %dx
	int $0x13
	jc read_loader

	jmp boot_entry
	

	jmp .
	
	// 引导结束段
	.section boot_end, "ax"
boot_sig: .byte 0x55, 0xaa 
