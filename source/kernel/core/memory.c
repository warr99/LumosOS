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
 * @param  page_dir 一级页表
 * @param  vaddr 虚拟起始地址
 * @param  paddr 物理起始地址
 * @param  count 页面数量
 * @param  perm 属性
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

    // 复制整个内核空间的页目录项，以便与其它进程共享内核空间
    // 用户空间的内存映射暂不处理，等加载程序时创建
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

int memory_alloc_for_page_dir(uint32_t page_dir, uint32_t vaddr, uint32_t size, int perm) {
    uint32_t curr_vaddr = vaddr;
    int page_count = up2(size, MEM_PAGE_SIZE) / MEM_PAGE_SIZE;
    vaddr = down2(vaddr, MEM_PAGE_SIZE);

    // 逐页分配内存，然后建立映射关系
    for (int i = 0; i < page_count; i++) {
        // 分配需要的内存
        uint32_t paddr = addr_alloc_page(&paddr_alloc, 1);
        if (paddr == 0) {
            log_printf("mem alloc failed. no memory");
            return 0;
        }

        // 建立分配的内存与指定地址的关联
        int err = memory_create_map((pde_t*)page_dir, curr_vaddr, paddr, 1, perm);
        if (err < 0) {
            log_printf("create memory map failed. err = %d", err);
            addr_free_page(&paddr_alloc, vaddr, i + 1);
            return -1;
        }

        curr_vaddr += MEM_PAGE_SIZE;
    }

    return 0;
}

int memory_alloc_page_for(uint32_t addr, uint32_t size, int perm) {
    return memory_alloc_for_page_dir(task_current()->tss.cr3, addr, size, perm);
}

uint32_t memory_alloc_one_page(void) {
    uint32_t addr = addr_alloc_page(&paddr_alloc, 1);
    return addr;
}

static pde_t* curr_page_dir(void) {
    return (pde_t*)(task_current()->tss.cr3);
}

void memory_free_one_page(uint32_t addr) {
    if (addr < MEM_TASK_BASE) {
        addr_free_page(&paddr_alloc, addr, 1);
    } else {
        pte_t* pte = find_pte(curr_page_dir(), addr, 0);
        ASSERT(pte == (pte_t*)0 && pte->present);
        addr_free_page(&paddr_alloc, pte_paddr(pte), 1);
        pte->v = 0;
    }
}

/**
 * @brief 销毁用户空间内存
 */
void memory_destroy_uvm(uint32_t page_dir) {
    uint32_t user_pde_start = pde_index(MEM_TASK_BASE);
    pde_t* pde = (pde_t*)page_dir + user_pde_start;

    // 释放页表中对应的各项，不包含映射的内核页面
    for (int i = user_pde_start; i < PDE_COUNT; i++, pde++) {
        if (!pde->present) {
            continue;
        }

        // 释放页表对应的物理页 + 页表
        pte_t* pte = (pte_t*)pde_paddr(pde);
        for (int j = 0; j < PTE_COUNT; j++, pte++) {
            if (!pte->present) {
                continue;
            }

            addr_free_page(&paddr_alloc, pte_paddr(pte), 1);
        }

        addr_free_page(&paddr_alloc, (uint32_t)pde_paddr(pde), 1);
    }

    // 页目录表
    addr_free_page(&paddr_alloc, page_dir, 1);
}

/**
 * @brief 复制页表及其所有的内存空间
 */
uint32_t memory_copy_uvm(uint32_t page_dir) {
    // 复制基础页表
    uint32_t to_page_dir = memory_create_uvm();
    if (to_page_dir == 0) {
        goto copy_uvm_failed;
    }

    // 再复制用户空间的各项
    uint32_t user_pde_start = pde_index(MEM_TASK_BASE);
    pde_t* pde = (pde_t*)page_dir + user_pde_start;

    // 遍历用户空间页目录项
    for (int i = user_pde_start; i < PDE_COUNT; i++, pde++) {
        if (!pde->present) {
            continue;
        }

        // 遍历页表
        pte_t* pte = (pte_t*)pde_paddr(pde);
        for (int j = 0; j < PTE_COUNT; j++, pte++) {
            if (!pte->present) {
                continue;
            }

            // 分配物理内存
            uint32_t page = addr_alloc_page(&paddr_alloc, 1);
            if (page == 0) {
                goto copy_uvm_failed;
            }

            // 建立映射关系
            uint32_t vaddr = (i << 22) | (j << 12);
            int err = memory_create_map((pde_t*)to_page_dir, vaddr, page, 1, get_pte_perm(pte));
            if (err < 0) {
                goto copy_uvm_failed;
            }

            // 复制内容。
            kernel_memcpy((void*)page, (void*)vaddr, MEM_PAGE_SIZE);
        }
    }
    return to_page_dir;

copy_uvm_failed:
    if (to_page_dir) {
        memory_destroy_uvm(to_page_dir);
    }
    return -1;
}

uint32_t memory_get_paddr(uint32_t page_dir, uint32_t vaddr) {
    pte_t* pte = find_pte((pde_t*)page_dir, vaddr, 0);
    if (pte == (pte_t*)0) {
        return 0;
    }
    return pte_paddr(pte) + (vaddr & (MEM_PAGE_SIZE - 1));
}

int memory_copy_uvm_data(uint32_t to, uint32_t page_dir, uint32_t from, uint32_t size) {
    char *buf, *pa0;

    while (size > 0) {
        // 获取目标的物理地址, 也即其另一个虚拟地址
        uint32_t to_paddr = memory_get_paddr(page_dir, to);
        if (to_paddr == 0) {
            return -1;
        }

        // 计算当前可拷贝的大小
        uint32_t offset_in_page = to_paddr & (MEM_PAGE_SIZE - 1);
        uint32_t curr_size = MEM_PAGE_SIZE - offset_in_page;
        if (curr_size > size) {
            curr_size = size;  // 如果比较大，超过页边界，则只拷贝此页内的
        }

        kernel_memcpy((void*)to_paddr, (void*)from, curr_size);

        size -= curr_size;
        to += curr_size;
        from += curr_size;
    }

    return 0;
}