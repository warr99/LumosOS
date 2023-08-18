/*
 * @Author: warrior
 * @Date: 2023-08-16 14:44:23
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-18 15:43:36
 * @Description: 
 */
/*
 * @Author: warrior
 * @Date: 2023-08-16 14:44:23
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-18 15:43:23
 * @Description: 
 */
#include "fs/file.h"
#include "ipc/mutex.h"
#include "tools/klib.h"

static file_t file_table[FILE_TABLE_SIZE];  // 系统中可打开的文件表
static mutex_t file_alloc_mutex;            // 访问file_table的互斥信号量

file_t* file_alloc(void) {
    file_t* file = (file_t*)0;

    mutex_lock(&file_alloc_mutex);
    for (int i = 0; i < FILE_TABLE_SIZE; i++) {
        file_t* p_file = file_table + i;
        if (p_file->ref == 0) {
            kernel_memset(p_file, 0, sizeof(file_t));
            p_file->ref = 1;
            file = p_file;
            break;
        }
    }
    mutex_unlock(&file_alloc_mutex);
    return file;
}

void file_free(file_t* file) {
    mutex_lock(&file_alloc_mutex);
    if (file->ref) {
        file->ref--;
    }
    mutex_unlock(&file_alloc_mutex);
}

void file_table_init(void) {
    // 文件描述符表初始化
    kernel_memset(&file_table, 0, sizeof(file_table));
    mutex_init(&file_alloc_mutex);
}


void file_inc_ref (file_t * file) {
    mutex_lock(&file_alloc_mutex);
	file->ref++;
    mutex_unlock(&file_alloc_mutex);
}