/*
 * @Author: warrior
 * @Date: 2023-08-07 16:14:57
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-19 16:11:36
 * @Description:
 */
#ifndef FS_H
#define FS_H

#include <sys/stat.h>
#include "fs/file.h"
#include "ipc/mutex.h"
#include "tools/list.h"

#define FS_MOUNTP_SIZE 512

struct _fs_t;

/**
 * @brief 文件系统操作接口
 */
typedef struct _fs_op_t {
    int (*mount)(struct _fs_t* fs, int major, int minor);           // 挂载
    void (*unmount)(struct _fs_t* fs);                              // 取消挂载
    int (*open)(struct _fs_t* fs, const char* path, file_t* file);  // 打开文件
    int (*read)(char* buf, int size, file_t* file);                 // 读取文件
    int (*write)(char* buf, int size, file_t* file);                // 写文件
    void (*close)(file_t* file);                                    // 关闭文件
    int (*seek)(file_t* file, uint32_t offset, int dir);            // 移动文件指针到文件中的特定位置
    int (*stat)(file_t* file, struct stat* st);                     // 获取文件或文件系统对象的元数据信息
} fs_op_t;

typedef enum _fs_type_t {
    FS_DEVFS,
} fs_type_t;

typedef struct _fs_t {
    char mount_point[FS_MOUNTP_SIZE];  // 文件系统名称
    fs_op_t* op;                       // 文件系统操作接口
    fs_type_t type;                    // 文件系统类型
    void* data;                        // 扩充数据
    int dev_id;                        // 设备id
    list_node_t node;                  // 链表节点
    mutex_t* mutex;                    // 互斥锁
} fs_t;

/**
 * @brief 转换目录为数字
 * @param path 目录
 * @param num 存储结果
 */
int path_to_num(const char* path, int* num);

/**
 * @brief 获取下一级子目录
 * @param path 目录
 */
const char* path_next_child(const char* path);

/**
 * @brief 判断路径是否以str开头
 * @param path 路径
 * @param str str
 * @return 1 or 0
 */
int path_begin_with(const char* path, const char* str);

    /**
     * @brief 文件系统初始化
     */
    void fs_init(void);

/**
 * 打开一个文件或者创建一个文件描述符。
 *
 * @param name  文件的路径和名称。
 * @param flag  打开文件的标志。标志可以是 O_RDONLY（只读），O_WRONLY（只写），O_RDWR（读写）等。
 * @param ...   可选的模式参数，通常在创建文件时使用，用于设置文件的访问权限。
 *
 * @return      如果文件成功打开或者创建，将返回一个非负整数作为文件描述符；如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_open(const char* name, int flag, ...);

/**
 * 从文件描述符读取数据到缓冲区中。
 *
 * @param file  文件描述符，要读取数据的文件。
 * @param ptr   指向保存读取数据的缓冲区的指针。
 * @param len   要读取的字节数。
 *
 * @return      成功时返回实际读取的字节数，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_read(int file, char* ptr, int len);

/**
 * 将数据从缓冲区写入文件。
 *
 * @param file  文件描述符，要写入数据的文件。
 * @param ptr   指向包含要写入文件的数据的缓冲区的指针。
 * @param len   要写入的字节数。
 *
 * @return      成功时返回实际写入的字节数，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_write(int file, char* ptr, int len);

/**
 * 移动文件读/写指针到指定位置。
 *
 * @param file  文件描述符，要移动指针的文件。
 * @param ptr   相对于dir的偏移量，用于指定移动的位置。
 * @param dir   指针移动的方向。可以是SEEK_SET（从文件开头开始偏移），SEEK_CUR（从当前指针位置偏移），或者SEEK_END（从文件末尾开始偏移）。
 *
 * @return      成功时返回新的文件指针位置（以字节为单位），如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_lseek(int file, int ptr, int dir);

/**
 * 关闭一个文件描述符。
 *
 * @param file  要关闭的文件描述符。
 *
 * @return      成功时返回0，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_close(int file);

/**
 * 判断文件描述符与tty关联
 */
int sys_isatty(int file);

/**
 * 查询文件的元数据
*/
int sys_fstat(int file, struct stat* st);

/**
 * 复制一个文件描述符
*/
int sys_dup(int file);

#endif