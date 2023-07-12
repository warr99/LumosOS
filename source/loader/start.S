/*
 * @Author: warrior
 * @Date: 2023-07-11 10:52:56
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-12 16:12:17
 * @Description: 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * 该部分程序存储于磁盘的第1个扇区，在计算机启动时将会由BIOS加载到0x7c00处
 * 之后，将由BIOS跳转至0x7c00处开始运行
 */



  	// 16位代码,务必加上
  	.code16
 	.text
	.global _start
	.extern loader_entry
_start:
	jmp loader_entry

  	.code32
	.text
	.global protect_mode_entry
	.extern load_kernel

protect_mode_entry:
	mov	$16 ,%ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %ss
	mov %ax, %fs
	mov %ax, %gs
	jmp $8, $load_kernel