/*
 * @Author: warrior
 * @Date: 2023-07-21 21:35:16
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-21 21:47:20
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
 * @brief: 将位图数转化为字节数
 * @param {int} bit_count 位图数
 * @return {int} 字节数
 */
int bitmap_byte_count(int bit_count);

/**
 * @brief: 初始化位图
 * @param {bitmap_t*} bitmap bitmap
 * @param {uint8_t*} bits bits
 * @param {int} count count
 * @param {int} init_bit init_bit
 * @return {*}
 */
void bitmap_init(bitmap_t* bitmap, uint8_t* bits, int count, int init_bit);

#endif