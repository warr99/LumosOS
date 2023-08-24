#include "fs/fatfs/fatfs.h"
#include "core/memory.h"
#include "dev/dev.h"
#include "fs/fs.h"
#include "tools/klib.h"
#include "tools/log.h"

/**
 * @brief 缓存读取磁盘数据，用于目录的遍历等
 */
static int bread_sector(fat_t* fat, int sector) {
    if (sector == fat->curr_sector) {
        return 0;
    }

    int cnt = dev_read(fat->fs->dev_id, sector, fat->fat_buffer, 1);
    if (cnt == 1) {
        fat->curr_sector = sector;
        return 0;
    }
    return -1;
}

/**
 * @brief 获取文件类型
 */
file_type_t diritem_get_type(diritem_t* item) {
    if (item->DIR_Attr & (DIRITEM_ATTR_VOLUME_ID | DIRITEM_ATTR_HIDDEN | DIRITEM_ATTR_SYSTEM)) {
        return FILE_UNKNOWN;
    }
    if (item->DIR_Attr & (DIRITEM_ATTR_LONG_NAME) == DIRITEM_ATTR_LONG_NAME) {
        return FILE_UNKNOWN;
    }
    return item->DIR_Attr & DIRITEM_ATTR_DIRECTORY ? FILE_DIR : FILE_NORMAL;
}

void diritem_get_name(diritem_t* item, char* dest) {
    char* c = dest;
    // 拓展名的位置
    char* ext = (char*)0;
    kernel_memset(dest, 0, SFN_LEN + 1);  // 最多11个字符 + 一个'.'
    for (int i = 0; i < 11; i++) {
        if (item->DIR_Name[i] != ' ') {
            *c++ = item->DIR_Name[i];
        }
        if (i == 7) {
            ext = c;
            *c++ = '.';
        }
    }
    if (ext && (ext[1] == '\0')) {
        ext[0] = '\0';
    }
}

static diritem_t* read_dir_entry(fat_t* fat, int index) {
    if ((index < 0) || (index >= fat->root_ent_cnt)) {
        return (diritem_t*)0;
    }
    int offset = index * sizeof(diritem_t);
    int err = bread_sector(fat, fat->root_start + offset / fat->bytes_per_sec);
    if (err < 0) {
        return (diritem_t*)0;
    }
    return (diritem_t*)(fat->fat_buffer + offset % fat->bytes_per_sec);
}

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
    fat->curr_sector = -1;
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
 * @brief 打开目录。只是简单地读取位置重设为0
 */
int fatfs_opendir(struct _fs_t* fs, const char* name, DIR* dir) {
    dir->index = 0;
    return 0;
}

/**
 * @brief 读取一个目录项
 */
int fatfs_readdir(struct _fs_t* fs, DIR* dir, struct dirent* dirent) {
    fat_t* fat = (fat_t*)fs->data;

    // 做一些简单的判断，检查
    while (dir->index < fat->root_ent_cnt) {
        diritem_t* item = read_dir_entry(fat, dir->index);
        if (item == (diritem_t*)0) {
            return -1;
        }
        // 结束项，不需要再扫描了，同时index也不能往前走
        if (item->DIR_Name[0] == DIRITEM_NAME_END) {
            break;
        }
        // 只显示普通文件和目录，其它的不显示
        if (item->DIR_Name[0] != DIRITEM_NAME_FREE) {
            file_type_t type = diritem_get_type(item);
            if ((type == FILE_NORMAL) || (type == FILE_DIR)) {
                dirent->index = dir->index++;
                dirent->type = diritem_get_type(item);
                dirent->size = item->DIR_FileSize;
                diritem_get_name(item, dirent->name);
                return 0;
            }
        }
        dir->index++;
    }
    return -1;
}

/**
 * @brief 关闭文件扫描读取
 */
int fatfs_closedir(struct _fs_t* fs, DIR* dir) {
    return 0;
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

    .opendir = fatfs_opendir,
    .readdir = fatfs_readdir,
    .closedir = fatfs_closedir,
};