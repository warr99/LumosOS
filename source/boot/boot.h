/*
 * @Author: Xr.Chen
 * @Date: 2023-07-11 11:33:41
 * @Last Modified by: Xr.Chen
 * @Last Modified time: 2023-07-11 11:34:42
 * @Description: 系统引导部分，启动时由硬件加载运行，然后完成对二级引导程序loader的加载
 * boot扇区容量较小，仅512字节。由于dbr占用了不少字节，导致其没有多少空间放代码，
 * 所以功能只能最简化,并且要开启最大的优化-os
 */
#ifndef BOOT_H
#define BOOT_H

#endif
