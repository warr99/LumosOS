#include "core/memory.h"
#include "tools/klib.h"
#include "tools/log.h"

static addr_alloc_t paddr_alloc;

static void addr_alloc_init(addr_alloc_t* alloc, uint8_t* bits, uint32_t start, uint32_t size, uint32_t page_size) {
    mutex_init(&alloc->mutex_t);
    alloc->start = start;
    alloc->size = size;
    alloc->page_size = page_size;
    bitmap_init(&alloc->bitmap, bits, alloc->size / page_size, 0);
}

/**
 * @brief 分配页
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
 * @brief 释放页
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

void show_memory_info(boot_info_t* boot_info) {
    log_printf("memory region:");
    for (int i = 0; i < boot_info->ram_region_count; i++) {
        log_printf("[%d]: 0x%x - 0x%x", i,
                   boot_info->ram_region_cfg[i].start,
                   boot_info->ram_region_cfg[i].size);
    }
    log_printf("\n");
}

static uint32_t total_men_size(boot_info_t* boot_info) {
    uint32_t men_size = 0;
    for (int i = 0; i < boot_info->ram_region_count; i++) {
        men_size += boot_info->ram_region_cfg[i].size;
    }
    return men_size;
}

void memory_init(boot_info_t* boot_info) {
    // 放置完 kernel 之后地址, 在链接脚本中定义
    extern uint8_t* mem_free_start;
    uint8_t* mem_free = (uint8_t*)&mem_free_start;
    log_printf("memory init...");
    show_memory_info(boot_info);
    // 1MB以上空间的空闲内存容量
    uint32_t mem_up1MB_free = total_men_size(boot_info) - MEM_EXT_START;
    // 对齐的页边界
    mem_up1MB_free = down2(mem_up1MB_free, MEN_PAGE_SIZE);
    log_printf("free memory: 0x%x, size: 0x%x", MEM_EXT_START, mem_up1MB_free);
    // 初始化位图缓冲区,也即虚拟地址-物理地址映射关系表,放置在 mem_free 处
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free, MEN_PAGE_SIZE);
    // 将 mem_free 往后移动,也即跳过虚拟地址-物理地址映射关系表,方便之后使用
    mem_free += bitmap_byte_count(paddr_alloc.size / MEN_PAGE_SIZE);
    // mem_free应该比EBDA地址要小
    ASSERT(mem_free < (uint8_t *)MEM_EBDA_START);

}