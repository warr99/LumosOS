#include "core/memory.h"
#include "cpu/mmu.h"
#include "tools/klib.h"
#include "tools/log.h"

static addr_alloc_t paddr_alloc;

static pde_t kernel_page_dir[PDE_COUNT] __attribute__((aligned(MEM_PAGE_SIZE)));

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

pte_t* find_pte(pde_t* page_dir, uint32_t vaddr, int alloc) {
    pte_t* page_table;
    // 一级页表的表项 = 一级页表起始位置 + 一级页表索引
    pde_t* pde = page_dir + pde_index(vaddr);
    // 如果表项存在
    if (pde->present) {
        // 从表项中取出二级页表的物理地址
        page_table = (pte_t*)pde_paddr(pde);
    } else if (alloc == 0) {
        return (pte_t*)0;
    } else if (alloc == 1) {
        // 不存在且 alloc == 1 ,申请表项
        uint32_t pg_paddr = addr_alloc_page(&paddr_alloc, 1);
        if (pg_paddr == 0) {
            return (pte_t*)0;
        }
        // 将申请的空间給页目录表(也即申请了一个二级页表)
        pde->v = pg_paddr | PDE_P | PDE_W | PDE_U;
        page_table = (pte_t*)pg_paddr;
        // 清空这个二级页表
        kernel_memset(page_table, 0, MEM_PAGE_SIZE);
    }

    // 返回要找的二级页表表项所在位置
    return page_table + pte_index(vaddr);
}

/**
 * @brief 将指定的地址空间进行映射
 * @param {pde_t*} page_dir 一级页表
 * @param {uint32_t} vaddr 虚拟起始地址
 * @param {uint32_t} paddr 物理起始地址
 * @param {int} count 页面数量
 * @param {uint32_t} perm 属性
 * @return {*} -1 失败
 */
int memory_create_map(pde_t* page_dir, uint32_t vaddr, uint32_t paddr, int count, uint32_t perm) {
    for (int i = 0; i < count; i++) {
        // log_printf("create map: v-0x%x p-0x%x, perm: 0x%x", vaddr, paddr, perm);
        // 找到对应的二级页表表项
        pte_t* pte = find_pte(page_dir, vaddr, 1);
        if (pte == (pte_t*)0) {
            // log_printf("create pte failed. pte == 0");
            return -1;
        }
        // log_printf("\tpte addr: 0x%x", (uint32_t)pte);
        ASSERT(pte->present == 0);
        // 设置二级页表表项的值(也即指定的物理地址和属性)
        pte->v = paddr | perm | PTE_P;
        vaddr += MEM_PAGE_SIZE;
        paddr += MEM_PAGE_SIZE;
    }
}

/**
 * @brief 创建内核页表,内核的物理地址在kernel.lds已经指定,必须映射到指定的物理地址(逻辑地址 -> 物理地址)
 * @return {void}
 */
void create_kernel_table(void) {
    extern uint8_t s_text[], e_text[], s_data[], kernel_base[];
    // 内核逻辑地址 物理地址映射表
    static memory_map_t kernel_map[] = {
        {kernel_base, s_text, kernel_base, PTE_W},       // 内核栈区
        {s_text, e_text, s_text, 0},                     // 内核代码区
        {s_data, (void*)MEM_EBDA_START, s_data, PTE_W},  // 内核数据区

        // 扩展存储空间一一映射，方便直接操作
        {(void*)MEM_EXT_START, (void*)MEM_EXT_END, (void*)MEM_EXT_START, PTE_W},
    };

    for (int i = 0; i < sizeof(kernel_map) / sizeof(memory_map_t); i++) {
        memory_map_t* map = kernel_map + i;
        // 逻辑地址起始位置
        uint32_t vstart = down2((uint32_t)map->vstart, MEM_PAGE_SIZE);
        // 逻辑地址终止位置
        uint32_t vend = up2((uint32_t)map->vend, MEM_PAGE_SIZE);
        // 指定的物理地址
        uint32_t pstart = down2((uint32_t)map->pstart, MEM_PAGE_SIZE);
        // 所需的页表数量
        int page_count = (vend - vstart) / MEM_PAGE_SIZE;
        memory_create_map(kernel_page_dir, vstart, pstart, page_count, map->perm);
    }
}

uint32_t memory_create_uvm(void) {
    pde_t* page_dir = (pde_t*)addr_alloc_page(&paddr_alloc, 1);
    if (page_dir == 0) {
        return 0;
    }
    kernel_memset((void*)page_dir, 0, MEM_PAGE_SIZE);
    uint32_t user_pde_start = pde_index(MEM_TASK_BASE);
    for (int i = 0; i < user_pde_start; i++) {
        page_dir[i].v = kernel_page_dir[i].v;
    }
    return (uint32_t)page_dir;
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
    mem_up1MB_free = down2(mem_up1MB_free, MEM_PAGE_SIZE);
    log_printf("free memory: 0x%x, size: 0x%x", MEM_EXT_START, mem_up1MB_free);
    // 初始化位图缓冲区,也即虚拟地址-物理地址映射关系表,放置在 mem_free 处
    addr_alloc_init(&paddr_alloc, mem_free, MEM_EXT_START, mem_up1MB_free, MEM_PAGE_SIZE);
    // 将 mem_free 往后移动,也即跳过虚拟地址-物理地址映射关系表,方便之后使用
    mem_free += bitmap_byte_count(paddr_alloc.size / MEM_PAGE_SIZE);
    // mem_free应该比EBDA地址要小
    ASSERT(mem_free < (uint8_t*)MEM_EBDA_START);
    // 创建内核页表
    create_kernel_table();
    // 打开二级分页机制
    mmu_set_page_dir((uint32_t)kernel_page_dir);
}