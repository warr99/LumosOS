/*
 * @Author: warrior
 * @Date: 2023-08-19 10:34:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 11:40:14
 * @Description:
 */
#include "fs/devfs/devfs.h"
#include "dev/dev.h"
#include "fs/file.h"
#include "fs/fs.h"

/**
 * @brief 挂载设备文件系统
 * @param  fs 设备文件系统
 * @param  major 主设备号
 * @param  minor 次设备号
 * @return {*}
 */
int devfs_mount(struct _fs_t* fs, int major, int minor) {
    fs->type = FS_DEVFS;
}

/**
 * @brief 取消设备文件系统的挂载
 * @param  fs 设备文件系统
 */
void devfs_unmount(struct _fs_t* fs) {
}

/**
 * @brief 打开指定的设备以进行读写
 * @param fs 设备文件系统
 * @param path 路径
 * @param file 文件
 * @return {*}
 */
int devfs_open(struct _fs_t* fs, const char* path, file_t* file) {
    return 0;
}

/**
 * @brief 读写指定的文件系统
 */
int devfs_read(char* buf, int size, file_t* file) {
    return dev_read(file->dev_id, file->pos, buf, size);
}

/**
 * @brief 写设备文件系统
 */
int devfs_write(char* buf, int size, file_t* file) {
    return dev_write(file->dev_id, file->pos, buf, size);
}

/**
 * @brief 关闭设备文件
 */
void devfs_close(file_t* file) {
    dev_close(file->dev_id);
}

/**
 * @brief 文件读写定位
 */
int devfs_seek(file_t* file, uint32_t offset, int dir) {
    return -1;  // 不支持定位
}

/**
 * @brief 获取文件信息
 */
int devfs_stat(file_t* file, struct stat* st) {
    return -1;
}

// 设备文件系统
fs_op_t devfs_op = {
    .mount = devfs_mount,
    .unmount = devfs_unmount,
    .open = devfs_open,
    .read = devfs_read,
    .write = devfs_write,
    .seek = devfs_seek,
    .stat = devfs_stat,
    .close = devfs_close,
};
