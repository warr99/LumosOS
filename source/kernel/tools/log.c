/*
 * @Author: warrior
 * @Date: 2023-07-17 10:04:20
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-28 19:38:06
 * @Description:
 */
#include "tools/log.h"
#include <stdarg.h>
#include "comm/cpu_instr.h"
#include "cpu/irq.h"
#include "dev/console.h"
#include "dev/dev.h"
#include "ipc/mutex.h"
#include "tools/klib.h"

#define LOG_USE_COM 0
#define COM1_PORT 0x3F8

static int log_dev_id;
static mutex_t mutex;

void log_init(void) {
    mutex_init(&mutex);
    log_dev_id = dev_open(DEV_TTY, 0, (void*)0);
#if LOG_USE_COM
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x80);
    outb(COM1_PORT + 0, 0x3);
    outb(COM1_PORT + 1, 0x00);
    outb(COM1_PORT + 3, 0x03);
    outb(COM1_PORT + 2, 0xc7);
    outb(COM1_PORT + 4, 0x0F);
#endif
}

void log_printf(const char* fmt, ...) {
    char str_buf[128];
    va_list args;

    kernel_memset(str_buf, '\0', sizeof(str_buf));
    va_start(args, fmt);
    kernel_vsprintf(str_buf, fmt, args);
    va_end(args);

    mutex_lock(&mutex);
    const char* p = str_buf;
#if LOG_USE_COM
    while (*p != '\0') {
        while ((inb(COM1_PORT + 5) & (1 << 6)) == 0) {
        };
        outb(COM1_PORT, *p++);
    }

    outb(COM1_PORT, '\r');
    outb(COM1_PORT, '\n');

#else
    // console_write(0, str_buf, kernel_strlen(str_buf));
    // dev_write(log_dev_id, 0, "log: ", 5);
    dev_write(log_dev_id, 0, str_buf, kernel_strlen(str_buf));
    char c = '\n';
    // console_write(0, &c, 1);
    dev_write(log_dev_id, 0, &c, 1);

#endif
    mutex_unlock(&mutex);
}