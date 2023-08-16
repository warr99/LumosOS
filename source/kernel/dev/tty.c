/*
 * @Author: warrior
 * @Date: 2023-08-15 10:32:56
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-16 13:50:45
 * @Description: 该文件包含与TTY设备交互相关的函数。
 */
#include "dev/tty.h"
#include "dev/console.h"
#include "dev/dev.h"
#include "dev/kbd.h"
#include "tools/log.h"

static tty_t tty_devs[TTY_NR];

static tty_t* get_tty(device_t* dev) {
    int tty = dev->minor;
    if ((tty < 0) || (tty >= TTY_NR) || (!dev->open_count)) {
        log_printf("tty is not opened. tty = %d", tty);
        return (tty_t*)0;
    }

    return tty_devs + tty;
}

/**
 * @brief 从缓冲区取数据
 * @param  fifo 缓冲区
 * @param  c 写到 c
 * @return 成功 0 失败 -1
 */
int tty_fifo_get(tty_fifo_t* fifo, char* c) {
    if (fifo->count <= 0) {
        return -1;
    }

    *c = fifo->buf[fifo->read++];
    if (fifo->read >= fifo->size) {
        fifo->read = 0;
    }
    fifo->count--;
    return 0;
}

/**
 * @brief 往缓冲区写数据
 * @param  fifo 缓冲区
 * @param  c 要写入的 c
 * @return 成功 0 失败 -1
 */
int tty_fifo_put(tty_fifo_t* fifo, char c) {
    if (fifo->count >= fifo->size) {
        return -1;
    }

    fifo->buf[fifo->write++] = c;
    if (fifo->write >= fifo->size) {
        fifo->write = 0;
    }
    fifo->count++;

    return 0;
}

/**
 * @brief FIFO初始化
 * @param  fifo fifo缓存
 * @param  buf  缓存区域
 * @param  size 大小
 * @return {*}
 */
void tty_fifo_init(tty_fifo_t* fifo, char* buf, int size) {
    fifo->buf = buf;
    fifo->count = 0;
    fifo->size = size;
    fifo->read = fifo->write = 0;  // 读索引，写索引
}

/**
 * @brief 打开TTY设备以进行通信。
 *
 * @param dev 代表TTY设备的设备结构。
 * @return 成功返回0，失败返回错误代码。
 */
int tty_open(device_t* dev) {
    int index = dev->minor;
    if ((index < 0) || (index > TTY_NR)) {
        log_printf("open tty failed. incorrect tty num = %d", index);
        return -1;
    }
    tty_t* tty = tty_devs + index;
    tty_fifo_init(&tty->ofifo, tty->obuf, TTY_OBUF_SIZE);
    sem_init(&tty->osem, TTY_OBUF_SIZE);
    tty_fifo_init(&tty->ififo, tty->ibuf, TTY_IBUF_SIZE);
    tty->console_index = index;
    tty->oflags = TTY_OCRLF;
    kbd_init();
    console_init(index);
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
    if (size < 0) {
        return -1;
    }
    tty_t* tty = get_tty(dev);
    if (!tty) {
        return -1;
    }
    int len = 0;
    while (size) {
        char c = *buf++;
        if ((c == '\n') && (tty->oflags & TTY_OCRLF)) {
            sem_wait(&tty->osem);
            int err = tty_fifo_put(&tty->ofifo, '\r');
            if (err < 0) {
                break;
            }
        }

        sem_wait(&tty->osem);
        int err = tty_fifo_put(&tty->ofifo, c);
        if (err < 0) {
            break;
        }
        len++;
        size--;
    }
    console_write(tty);
    return size;
}

/**
 * @brief 控制TTY设备。
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
