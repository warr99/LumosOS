/*
 * @Author: warrior
 * @Date: 2023-07-21 21:35:16
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-21 22:01:59
 * @Description:
 */
#ifndef BITMAP_H
#define BITMAP_H

#include "comm/types.h"

typedef struct _bitmap_t {
    int bit_count;
    uint8_t* bits;
} bitmap_t;

/**
 * @brief 将位图数转化为字节数
 * @param {int} bit_count 位图数
 * @return {int} 字节数
 */
int bitmap_byte_count(int bit_count);

/**
 * @brief 初始化位图
 * @param {bitmap_t*} bitmap bitmap
 * @param {uint8_t*} bits bits
 * @param {int} count count
 * @param {int} init_bit init_bit
 * @return {*}
 */
void bitmap_init(bitmap_t* bitmap, uint8_t* bits, int count, int init_bit);

/**
 * @brief 获取某些位的值
 * @param {bitmap_t*} bitmap 位图
 * @param {int} index 索引
 * @return {int} 值
 */
int bitmap_get_bit(bitmap_t* bitmap, int index);

/**
 * @brief 设置某些位的值
 * @param {bitmap_t*} bitmap 位图
 * @param {int} index 索引
 * @param {int} count 设置的数量
 * @param {int} bit 要设置的值
 * @return {*}
 */
void bitmap_set_bit(bitmap_t* bitmap, int index, int count, int bit);

/**
 * @brief 判断 index 对应的位是 0 还是 1
 * @param {bitmap_t*} bitmap 位图
 * @param {int} index 索引
 * @return {int} 0 or 1
 */
int bit_map_is_set(bitmap_t* bitmap, int index);

/**
 * @brief 分配内存中连续为 1 (或者0) 的位
 * @param {bitmap_t*} bitmap 位图
 * @param {int} bit 要找 0 还是 1
 * @param {int} count 数量
 * @return {*}
 */
int bitmap_alloc_nbits(bitmap_t* bitmap, int bit, int count);

#endif