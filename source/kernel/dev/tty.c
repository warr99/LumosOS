/*
 * @Author: warrior
 * @Date: 2023-08-15 10:32:56
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-15 10:52:46
 * @Description: 该文件包含与TTY设备交互相关的函数。
 */
#include "dev/tty.h"
#include "dev/dev.h"

/**
 * @brief 打开TTY设备以进行通信。
 *
 * @param dev 代表TTY设备的设备结构。
 * @return 成功返回0，失败返回错误代码。
 */
int tty_open(device_t* dev) {
    return 0;
}

/**
 * @brief 从TTY设备读取数据。
 *
 * @param dev 代表TTY设备的设备结构。
 * @param addr 要读取的地址。
 * @param buf 用于存储读取数据的缓冲区。
 * @param size 缓冲区大小和要读取的数据量。
 * @return 读取的字节数。
 */
int tty_read(device_t* dev, int addr, char* buf, int size) {
    return size;
}

/**
 * @brief 向TTY设备写入数据。
 *
 * @param dev 代表TTY设备的设备结构。
 * @param addr 要写入的地址。
 * @param buf 包含要写入数据的缓冲区。
 * @param size 要写入的数据大小。
 * @return 写入的字节数。
 */
int tty_write(device_t* dev, int addr, char* buf, int size) {
    return size;
}

/**
 * @brief 控制TTY设备的各个方面。
 *
 * @param dev 代表TTY设备的设备结构。
 * @param cmd 命令代码，指定要执行的操作。
 * @param arg0 命令的第一个参数。
 * @param arg1 命令的第二个参数。
 * @return 成功返回0，失败返回错误代码。
 */
int tty_control(device_t* dev, int cmd, int arg0, int arg1) {
    return 0;
}

/**
 * @brief 关闭与TTY设备的通信。
 *
 * @param dev 代表TTY设备的设备结构。
 */
void tty_close(device_t* dev) {
}

/**
 * @brief TTY设备的描述符结构。
 */
dev_desc_t dev_tty_desc = {
    .name = "tty",
    .major = DEV_TTY,
    .open = tty_open,
    .read = tty_read,
    .write = tty_write,
    .control = tty_control,
    .close = tty_close,
};
