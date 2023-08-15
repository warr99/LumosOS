#include "dev/dev.h"

extern dev_desc_t dev_tty_desc;

/**
 * @brief 打开指定的设备
 * @param major 主设备号
 * @param minor 次设备号
 * @param data 需要传入的数据
 * @return dev_id
 */
int dev_open(int major, int minor, void* data) {
    return -1;
}

/**
 * @brief 读取指定字节的数据
 * @param dev_id dev_open() 返回的 dev_id
 * @param addr 读取的起始地址
 * @param buf 放到哪里
 * @param size 读取的数据量
 * @return 实际读取的数据量
 */
int dev_read(int dev_id, int addr, char* buf, int size) {
    return size;
}

/**
 * @brief 写指定字节的数据
 * @param dev_id dev_open() 返回的 dev_id
 * @param addr 写入的起始地址
 * @param buf 放到哪里
 * @param size 写入的数据量
 * @return 实际写入的数据量
 */
int dev_write(int dev_id, int addr, char* buf, int size) {
    return size;
}

/**
 * @brief 发送控制命令
 * @param dev_id dev_open() 返回的 dev_id
 * @param cmd 命令
 * @param arg0 参数0
 * @param arg1 参数1
 * @return 1 or 0
 */
int dev_control(int dev_id, int cmd, int arg0, int arg1) {
    return 0;
}

/**
 * @brief 关闭设备
 * @param dev_id dev_open() 返回的 dev_id
 */
void dev_close(int dev_id) {
}
