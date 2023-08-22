/*
 * @Author: warrior
 * @Date: 2023-08-19 10:34:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 14:34:20
 * @Description:
 */
/*
 * @Author: warrior
 * @Date: 2023-08-19 10:34:58
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 14:32:12
 * @Description:
 */
#include "fs/devfs/devfs.h"
#include "dev/dev.h"
#include "fs/file.h"
#include "fs/fs.h"
#include "tools/klib.h"
#include "tools/log.h"

// 设备文件系统中支持的设备
static devfs_type_t devfs_type_list[] = {
    // tty设备
    {
        .name = "tty",
        .dev_type = DEV_TTY,
        .file_type = FILE_TTY,
    }};

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
    // path = tty0
    // 遍历所有支持的设备类型列表，根据path中的路径，找到相应的设备类型
    for (int i = 0; i < sizeof(devfs_type_list) / sizeof(devfs_type_list[0]); i++) {
        devfs_type_t* type = devfs_type_list + i;
        int type_name_len = kernel_strlen(type->name);
        if (kernel_strncmp(path, type->name, type_name_len) == 0) {
            int minor;
            if ((kernel_strlen(path) > type_name_len) && (path_to_num(path + type_name_len, &minor)) < 0) {
                log_printf("Get device num failed. %s", path);
                break;
            }
            // 打开设备
            int dev_id = dev_open(type->dev_type, minor, (void*)0);
            if (dev_id < 0) {
                log_printf("Open device failed:%s", path);
                break;
            }
            file->dev_id = dev_id;
            file->fs = fs;
            file->pos = 0;
            file->size = 0;
            file->type = type->file_type;
            return 0;
        }
    }
    return -1;
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