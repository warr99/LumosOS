/*
 * @Author: warrior
 * @Date: 2023-07-24 11:29:15
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-24 15:26:32
 * @Description: 虚拟内存管理单元 -> 分页机制
 */
#ifndef MMU_H
#define MMU_H

#include "comm/cpu_instr.h"
#include "comm/types.h"

#define PDE_COUNT 1024
#define PTE_P (1 << 0)
#define PDE_P (1 << 0)
#define PTE_W (1 << 1)
#define PDE_W (1 << 1)
#define PDE_U (1 << 2)

/**
 * @brief 两级页表机制中的一级页表(页目录表)
 */
typedef union _pde_t {
    uint32_t v;  // 整个32位页目录表项的值
    struct {
        uint32_t present : 1;        // 0 (P) 存在位；必须为1才能映射4KB的页面
        uint32_t write_disable : 1;  // 1 (R/W) 读/写标志；如果为0,则禁止写入
        uint32_t user_mode_acc : 1;  // 2 (U/S) 用户模式访问标志；如果为0,不允许用户模式访问
        uint32_t write_through : 1;  // 3 (PWT) 页面级写穿透标志
        uint32_t cache_disable : 1;  // 4 (PCD) 页面级缓存禁止标志
        uint32_t accessed : 1;       // 5 (A) 已访问标志
        uint32_t : 1;                // 6 保留位,无意义,忽略
        uint32_t ps : 1;             // 7 (PS) 页大小标志
        uint32_t : 4;                // 11:8 保留位,无意义,忽略
        // 4GB = 2^32B,4KB = 2^12B,因此4GB的内存总共会被分为 2^32/2^12 = 2^20 个内存块(页框)，因此内存块号的范围应该是0~2^20-1,需要20位表示
        uint32_t phy_pt_addr : 20;  // 高20位 物理地址所在的的页号(物理地址 = 页号 * page_size + 偏移量)
    };
} pde_t;

/**
 * @brief 页表项（Page-Table Entry）
 */
typedef union _pte_t {
    uint32_t v;  // 整个32位页表项的值
    struct {
        uint32_t present : 1;         // 0 (P) 存在位；必须为1才能映射4KB的页面(:1 表示占一个bit)
        uint32_t write_disable : 1;   // 1 (R/W) 读/写标志；如果为0,则禁止写入
        uint32_t user_mode_acc : 1;   // 2 (U/S) 用户模式访问标志；如果为0,不允许用户模式访问
        uint32_t write_through : 1;   // 3 (PWT) 页面级写穿透标志
        uint32_t cache_disable : 1;   // 4 (PCD) 页面级缓存禁止标志
        uint32_t accessed : 1;        // 5 (A) 已访问标志
        uint32_t dirty : 1;           // 6 (D) 脏页标志
        uint32_t pat : 1;             // 7 PAT
        uint32_t global : 1;          // 8 (G) 全局标志
        uint32_t : 3;                 // 保留位,无意义,忽略
        uint32_t phy_page_addr : 20;  // 高20位 物理地址所在的的页号
    };
} pte_t;

static inline void mmu_set_page_dir(uint32_t paddr) {
    write_cr3(paddr);
}

/**
 * @brief 从逻辑地址中取出一级页表的索引
 * @param {uint32_t} vaddr 逻辑地址
 * @return {*} 一级页表的索引(10 10 12 -> 高10位)
 */
static inline uint32_t pde_index(uint32_t vaddr) {
    int index = (vaddr >> 22);
    return index;
}

/**
 * @brief 从逻辑地址中取出二级页表的索引
 * @param {uint32_t} vaddr 逻辑地址
 * @return {*} 一级页表的索引(10 10 12 -> 中间10位)
 */
static inline uint32_t pte_index(uint32_t vaddr) {
    int index = (vaddr >> 12) & 0x3FF;
    return index;
}

/**
 * @brief 取出一级页表中存储的二级页表的物理地址
 * @param {pde_t*} pde 一级页表
 * @return {*} 二级页表的物理地址
 */
static inline uint32_t pde_paddr(pde_t* pde) {
    return pde->phy_pt_addr << 12;
}

/**
 * @brief 取出二级页表中存储的物理地址
 * @param {pte_t*} pte 二级页表
 * @return {*} 物理地址
 */
static inline uint32_t pte_paddr(pte_t* pte) {
    return pte->phy_page_addr << 12;
}

#endif