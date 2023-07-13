/*
 * @Author: warrior
 * @Date: 2023-07-11 13:44:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-13 14:02:23
 * @Description:
 */

#include "comm/elf.h"
#include "loader.h"

static void read_dick(uint32_t sector, int sector_count, uint8_t* buf) {
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
    Elf32_Ehdr* elf_hdr = (Elf32_Ehdr*)file_buffer;
    if ((elf_hdr->e_ident[0] != 0x7F) || elf_hdr->e_ident[1] != 'E' || elf_hdr->e_ident[2] != 'L' || elf_hdr->e_ident[3] != 'F') {
        return 0;
    }
    for (int i = 0; i < elf_hdr->e_phnum; i++) {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(file_buffer + elf_hdr->e_phoff) + i;
        if (phdr->p_type != PT_LOAD) {
            continue;
        }
        uint8_t* src = file_buffer + phdr->p_offset;
        uint8_t* dest = (uint8_t*)phdr->p_paddr;
        for (int j = 0; j < phdr->p_filesz; j++) {
            *dest++ = *src++;
        }
        dest = (uint8_t*)phdr->p_paddr + phdr->p_filesz;
        for (int j = 0; i < phdr->p_memsz - phdr->p_filesz; j++) {
            *dest++ = 0;
        }
    }
    return elf_hdr->e_entry;
}

static void die(int code) {
    for (;;) {
    }
}
void load_kernel(void) {
    read_dick(100, 500, (uint8_t*)SYS_KERNEL_LOAD_ADDR);
    uint32_t kernel_entry = reload_elf_file((uint8_t*)SYS_KERNEL_LOAD_ADDR);
    if (kernel_entry == 0) {
        die(-1);
    }
    ((void (*)(boot_info_t*))kernel_entry)(&boot_info);
    for (;;) {
    }
}