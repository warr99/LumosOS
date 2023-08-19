#ifndef DEV_H
#define DEV_H

#define DEV_NAME_SZIE 32

enum {
    DEV_UNKNOWN = 0,  // 未知类型
    DEV_TTY = 1,
};

typedef struct _device_t {
    struct _dev_desc_t* desc;  // 设备特性描述符
    int mode;                  // 操作模式
    int minor;                 // 次设备号
    void* data;                // 设备参数
    int open_count;            // 打开次数
} device_t;

/**
 * @brief 某种类型设备描述结构
 */
typedef struct _dev_desc_t {
    char name[DEV_NAME_SZIE];  // 设备名称
    int major;                 // 主设备号

    int (*open)(device_t* dev);
    int (*read)(device_t* dev, int addr, char* buf, int size);
    int (*write)(device_t* dev, int addr, char* buf, int size);
    int (*control)(device_t* dev, int cmd, int arg0, int arg1);
    void (*close)(device_t* dev);
} dev_desc_t;

/**
 * @brief 打开指定的设备
 * @param major 主设备号
 * @param minor 次设备号
 * @param data 需要传入的数据
 * @return dev_id
 */
int dev_open(int major, int minor, void* data);

/**
 * @brief 读取指定字节的数据
 * @param dev_id dev_open() 返回的 dev_id
 * @param addr 读取的起始地址
 * @param buf 放到哪里
 * @param size 读取的数据量
 * @return 实际读取的数据量
 */
int dev_read(int dev_id, int addr, char* buf, int size);

/**
 * @brief 写指定字节的数据
 * @param dev_id dev_open() 返回的 dev_id
 * @param addr 写入的起始地址
 * @param buf 写入的数据
 * @param size 写入的数据量
 * @return 实际写入的数据量
 */
int dev_write(int dev_id, int addr, char* buf, int size);

/**
 * @brief 发送控制命令
 * @param dev_id dev_open() 返回的 dev_id
 * @param cmd 命令
 * @param arg0 参数0
 * @param arg1 参数1
 * @return 1 or 0
 */
int dev_control(int dev_id, int cmd, int arg0, int arg1);

/**
 * @brief 关闭设备
 * @param dev_id dev_open() 返回的 dev_id
 */
void dev_close(int dev_id);

#endif