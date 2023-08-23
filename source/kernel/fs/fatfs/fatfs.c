#include "fs/fatfs/fatfs.h"
#include "core/memory.h"
#include "dev/dev.h"
#include "fs/fs.h"
#include "tools/klib.h"
#include "tools/log.h"

/**
 * @brief 挂载fat文件系统
 */
int fatfs_mount(struct _fs_t* fs, int dev_major, int dev_minor) {
    int dev_id = dev_open(dev_major, dev_minor, (void*)0);
    if (dev_id < 0) {
        log_printf("open disk failed. major: %x, minor: %x", dev_major, dev_minor);
        return -1;
    }
    // 读取dbr扇区并进行检查
    dbr_t* dbr = (dbr_t*)memory_alloc_one_page();
    if (!dbr) {
        log_printf("mount fat failed: can't alloc buf.");
        goto mount_failed;
    }
    int cnt = dev_read(dev_id, 0, (char*)dbr, 1);
    if (cnt < 1) {
        log_printf("read dbr failed.");
        goto mount_failed;
    }
    fat_t* fat = &fs->fat_data;
    fat->fat_buffer = (uint8_t*)dbr;
    fat->bytes_per_sec = dbr->BPB_BytsPerSec;
    fat->tbl_start = dbr->BPB_RsvdSecCnt;
    fat->tbl_sectors = dbr->BPB_FATSz16;
    fat->tbl_cnt = dbr->BPB_NumFATs;
    fat->root_ent_cnt = dbr->BPB_RootEntCnt;
    fat->sec_per_cluster = dbr->BPB_SecPerClus;
    fat->cluster_byte_size = fat->sec_per_cluster * dbr->BPB_BytsPerSec;
    fat->root_start = fat->tbl_start + fat->tbl_sectors * fat->tbl_cnt;
    fat->data_start = fat->root_start + fat->root_ent_cnt * 32 / SECTOR_SIZE;
    fat->fs = fs;
    if (fat->tbl_cnt != 2) {
        log_printf("fat table num error, major: %x, minor: %x", dev_major, dev_minor);
        goto mount_failed;
    }

    if (kernel_memcmp(dbr->BS_FileSysType, "FAT16", 5) != 0) {
        log_printf("not a fat16 file system, major: %x, minor: %x", dev_major, dev_minor);
        goto mount_failed;
    }

    fs->type = FS_FAT16;
    fs->data = &fs->fat_data;
    fs->dev_id = dev_id;
    return 0;
mount_failed:
    if (dbr) {
        memory_free_one_page((uint32_t)dbr);
    }
    dev_close(dev_id);
    return -1;
}

/**
 * @brief 卸载fatfs文件系统
 */
void fatfs_unmount(struct _fs_t* fs) {
    fat_t* fat = (fat_t*)fs->data;
    dev_close(fs->dev_id);
    memory_free_one_page((uint32_t)fat->fat_buffer);
}

/**
 * @brief 打开指定的文件
 */
int fatfs_open(struct _fs_t* fs, const char* path, file_t* file) {
    return -1;
}

/**
 * @brief 读文件
 */
int fatfs_read(char* buf, int size, file_t* file) {
    return 0;
}

/**
 * @brief 写文件数据
 */
int fatfs_write(char* buf, int size, file_t* file) {
    return 0;
}

void fatfs_close(file_t* file) {
}

/**
 * @brief 文件读写位置的调整
 */
int fatfs_seek(file_t* file, uint32_t offset, int dir) {
    return -1;  // 不支持，只允许应用程序连续读取
}

int fatfs_stat(file_t* file, struct stat* st) {
    return -1;
}

/**
 * @brief FAT16 文件系统
 */
fs_op_t fatfs_op = {
    .mount = fatfs_mount,
    .unmount = fatfs_unmount,
    .open = fatfs_open,
    .read = fatfs_read,
    .write = fatfs_write,
    .seek = fatfs_seek,
    .stat = fatfs_stat,
    .close = fatfs_close,
};