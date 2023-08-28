/*
 * @Author: warrior
 * @Date: 2023-07-17 13:17:47
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-28 14:56:53
 * @Description: 内核依赖的工具类(字符串 内存操作)
 */
#ifndef KLIB_h
#define KLIB_h

#include <stdarg.h>
#include "comm/types.h"

/**
 * @brief 复制字符串
 * @param {char*} dest 目标字符串
 * @param {char*} src 源字符串
 * @return {*}
 */
void kernel_strcpy(char* dest, const char* src);

/**
 * @brief
 * @param {char*} dest 目标字符串
 * @param {char*} src 源字符串
 * @param {int} size 复制的范围
 * @return {*}
 */
void kernel_strncpy(char* dest, const char* src, int size);

/**
 * @brief  比较两个字符串是否相同
 * @param {char*} s1 字符串1
 * @param {char*} s2 字符串2
 * @param {int} size 比较范围
 * @return {int} 不一样返回1,一样返回0
 */
int kernel_strncmp(const char* s1, const char* s2, int size);

/**
 * @brief 字符串长度
 * @param {char*} str 字符串
 * @return {int} 字符串长度
 */
int kernel_strlen(const char* str);

/**
 * @brief 复制内存
 * @param {void*} dest 目标地址
 * @param {void*} src 源地址
 * @param {int} size 大小(字节)
 * @return {*}
 */
void kernel_memcpy(void* dest, void* src, int size);

/**
 * @brief 为内存地址设置值
 * @param {void*} dest 目标地址
 * @param {uint8_t} v 数据
 * @param {int} size 数据量
 * @return {*}
 */
void kernel_memset(void* dest, uint8_t v, int size);

/**
 * @brief 比较两个内存地址存储的值
 * @param {void*} d1 内存地址1
 * @param {void*} d2 内存地址2
 * @param {int} size 大小
 * @return {int} 不一样返回1,一样返回0
 */
int kernel_memcmp(void* d1, void* d2, int size);

void kernel_itoa(char* buf, int num, int base);

void kernel_vsprintf(char* buf, const char* fmt, va_list args);

void kernel_sprintf(char* buf, const char* fmt, ...);

/**
 * @brief 将 size 转化为 bound 的整数倍
 * @param {uint32_t} size size
 * @param {uint32_t} bound bound(2^n)
 * @return {uint32_t} size 最接近且小于等于 bound 的整数倍的结果
 */
static inline uint32_t down2(uint32_t size, uint32_t bound) {
    return size & ~(bound - 1);
}

/**
 * @brief 将 size 转化为 bound 的整数倍
 * @param  size size
 * @param  bound bound(2^n)
 * @return  最接近且大于等于 bound 的整数倍的结果
 */
static inline uint32_t up2(uint32_t size, uint32_t bound) {
    return (size + bound - 1) & ~(bound - 1);
}

#ifndef RELEASE

#define ASSERT(expr) \
    if (!(expr))     \
    pannic(__FILE__, __LINE__, __func__, #expr)
void pannic(const char* file, int line, const char* func, const char* cond);
#else
#define ASSERT() ((void)0)
#endif

/**
 * @brief 获取字符串数组中字符串的数量
 * @param start 字符串数组
 * @return int 字符串的数量
 */
int strings_count(char** start);

/**
 * @brief  获取完整路径中的文件名部分
 * @param  name 完整路径
 * @return char* 文件名部分
 */
char* get_file_name(const char* name);

#endif