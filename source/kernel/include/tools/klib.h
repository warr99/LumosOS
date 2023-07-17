/*
 * @Author: warrior
 * @Date: 2023-07-17 13:17:47
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-17 14:08:21
 * @Description: 内核依赖的工具类(字符串 内存操作)
 */
#ifndef KLIB_h
#define KLIB_h

#include "comm/types.h"

/**
 * @brief: 复制字符串
 * @param {char*} dest 目标字符串
 * @param {char*} src 源字符串
 * @return {*}
 */
void kernel_strcpy(char* dest, const char* src);

/**
 * @brief:
 * @param {char*} dest 目标字符串
 * @param {char*} src 源字符串
 * @param {int} size 复制的范围
 * @return {*}
 */
void kernel_strncpy(char* dest, const char* src, int size);

/**
 * @brief:  比较两个字符串是否相同
 * @param {char*} s1 字符串1
 * @param {char*} s2 字符串2
 * @param {int} size 比较范围
 * @return {int} 不一样返回1,一样返回0
 */
int kernel_strncmp(const char* s1, const char* s2, int size);

/**
 * @brief: 字符串长度
 * @param {char*} str 字符串
 * @return {int} 字符串长度
 */
int kernel_strlen(const char* str);

/**
 * @brief: 复制内存
 * @param {void*} dest 目标地址
 * @param {void*} src 源地址
 * @param {int} size 大小(字节)
 * @return {*}
 */
void kernel_memcpy(void* dest, void* src, int size);

/**
 * @brief: 为内存地址设置值
 * @param {void*} dest 目标地址
 * @param {uint8_t} v 数据
 * @param {int} size 数据量
 * @return {*}
 */
void kernel_memset(void* dest, uint8_t v, int size);

/**
 * @brief: 比较两个内存地址存储的值
 * @param {void*} d1 内存地址1
 * @param {void*} d2 内存地址2
 * @param {int} size 大小
 * @return {int} 不一样返回1,一样返回0
 */
int kernel_memcmp(void* d1, void* d2, int size);
#endif