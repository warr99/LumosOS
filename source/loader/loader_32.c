/*
 * @Author: warrior
 * @Date: 2023-07-11 13:44:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-23 23:37:16
 * @Description:
 */

#include "comm/elf.h"
#include "loader.h"

static void read_disk(int sector, int sector_count, uint8_t* buf) {
    outb(0x1F6, (uint8_t)(0xE0));

    outb(0x1F2, (uint8_t)(sector_count >> 8));
    outb(0x1F3, (uint8_t)(sector >> 24));  // LBA参数的24~31位
    outb(0x1F4, (uint8_t)(0));             // LBA参数的32~39位
    outb(0x1F5, (uint8_t)(0));             // LBA参数的40~47位

    outb(0x1F2, (uint8_t)(sector_count));
    outb(0x1F3, (uint8_t)(sector));        // LBA参数的0~7位
    outb(0x1F4, (uint8_t)(sector >> 8));   // LBA参数的8~15位
    outb(0x1F5, (uint8_t)(sector >> 16));  // LBA参数的16~23位

    outb(0x1F7, (uint8_t)0x24);

    // 读取数据
    uint16_t* data_buf = (uint16_t*)buf;
    while (sector_count-- > 0) {
        // 每次扇区读之前都要检查，等待数据就绪
        while ((inb(0x1F7) & 0x88) != 0x8) {
        }

        // 读取并将数据写入到缓存中
        for (int i = 0; i < SECTOR_SIZE / 2; i++) {
            *data_buf++ = inw(0x1F0);
        }
    }
}

static uint32_t reload_elf_file(uint8_t* file_buffer) {
    // 读取的只是ELF文件，不像BIN那样可直接运行，需要从中加载出有效数据和代码
    // 简单判断是否是合法的ELF文件
    Elf32_Ehdr* elf_hdr = (Elf32_Ehdr*)file_buffer;
    if ((elf_hdr->e_ident[0] != ELF_MAGIC) || (elf_hdr->e_ident[1] != 'E') || (elf_hdr->e_ident[2] != 'L') || (elf_hdr->e_ident[3] != 'F')) {
        return 0;
    }

    // 然后从中加载程序头，将内容拷贝到相应的位置
    for (int i = 0; i < elf_hdr->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(file_buffer + elf_hdr->e_phoff) + i;
        if (phdr->p_type != PT_LOAD) {
            continue;
        }

        // 全部使用物理地址，此时分页机制还未打开
        uint8_t* src = file_buffer + phdr->p_offset;
        uint8_t* dest = (uint8_t*)phdr->p_paddr;
        for (int j = 0; j < phdr->p_filesz; j++) {
            *dest++ = *src++;
        }

        // memsz和filesz不同时，后续要填0
        dest = (uint8_t*)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; j < phdr->p_memsz - phdr->p_filesz; j++) {
            *dest++ = 0;
        }
    }

    return elf_hdr->e_entry;
}

static void die(int code) {
    for (;;) {
    }
}

#define CR4_PSE (1 << 4)
#define CR0_PG (1 << 31)

#define PDE_P (1 << 0)
#define PDE_W (1 << 1)
#define PDE_PS (1 << 7)

void enable_page_mode(void) {
    // 页目录表
    static uint32_t page_dir[1024] __attribute__((aligned(4096))) = {
        // 初始化第0项,把逻辑地址的0~4M映射到物理地址的0~4M,因为loader已经被放置在1M以内的物理地址,不能改变
        [0] = PDE_P | PDE_PS | PDE_W,
    };

    // 设置 cr4 寄存器
    uint32_t cr4 = read_cr4();
    write_cr4(cr4 | CR4_PSE);

    // 设置 cr3 寄存器,写入页目录表地址
    write_cr3((uint32_t)page_dir);

    // 设置 cr0 寄存器,打开分页机制
    write_cr0(read_cr0() | CR0_PG);
}

void load_kernel(void) {
    read_disk(100, 500, (uint8_t*)SYS_KERNEL_LOAD_ADDR);
    uint32_t kernel_entry = reload_elf_file((uint8_t*)SYS_KERNEL_LOAD_ADDR);
    if (kernel_entry == 0) {
        die(-1);
    }
    // 打开分页机制
    enable_page_mode();
    ((void (*)(boot_info_t*))kernel_entry)(&boot_info);
    for (;;) {
    }
}