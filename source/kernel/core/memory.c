#include "core/memory.h"
#include "tools/log.h"

static void addr_alloc_init(addr_alloc_t* alloc, uint8_t* bits, uint32_t start, uint32_t size, uint32_t page_szie) {
    mutex_init(&alloc->mutex_t);
    alloc->start = start;
    alloc->size = size;
    alloc->page_size = page_szie;
    bitmap_init(&alloc->bitmap, bits, alloc->size / page_szie, 0);
}

/**
 * @brief: 分配页
 * @param {addr_alloc_t*} alloc 映射关系表
 * @param {int} page_count 页的数量
 * @return {uint32_t} 分配的页的起始地址
 */
static uint32_t addr_alloc_page(addr_alloc_t* alloc, int page_count) {
    mutex_lock(&alloc->mutex_t);
    uint32_t addr = -1;
    int page_index = bitmap_alloc_nbits(&alloc->bitmap, 0, page_count);
    if (page_index >= 0) {
        addr = alloc->start + page_index * alloc->page_size;
    }
    mutex_unlock(&alloc->mutex_t);
    return addr;
}

/**
 * @brief: 释放页
 * @param {addr_alloc_t*} alloc 映射关系表
 * @param {uint32_t} addr 要释放的页的起始地址
 * @param {int} page_count 页的数量
 * @return {*}
 */
static uint32_t addr_free_page(addr_alloc_t* alloc, uint32_t addr, int page_count) {
    mutex_lock(&alloc->mutex_t);
    uint32_t page_index = (addr - alloc->start) / alloc->page_size;
    bitmap_set_bit(&alloc->bitmap, page_index, page_count, 0);
    mutex_unlock(&alloc->mutex_t);
}

void memory_init(boot_info_t* boot_info) {
    addr_alloc_t addr_alloc;
    uint8_t bits[8];
    uint32_t addr;
    addr_alloc_init(&addr_alloc, bits, 0x1000, 64 * 4096, 4096);
    for (int i = 0; i < 32; i++) {
        addr = addr_alloc_page(&addr_alloc, 2);
        log_printf("alloc addr: 0x%x", addr);
    }
    addr = 0x1000;
    for (int i = 0; i < 32; i++) {
        addr_free_page(&addr_alloc, addr, 2);
        addr += 8192;
    }
}