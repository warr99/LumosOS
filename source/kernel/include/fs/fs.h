#ifndef FS_H
#define FS_H

/**
 * 打开一个文件或者创建一个文件描述符。
 *
 * @param name  文件的路径和名称。
 * @param flag  打开文件的标志。标志可以是 O_RDONLY（只读），O_WRONLY（只写），O_RDWR（读写）等。
 * @param ...   可选的模式参数，通常在创建文件时使用，用于设置文件的访问权限。
 *
 * @return      如果文件成功打开或者创建，将返回一个非负整数作为文件描述符；如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_open(const char* name, int flag, ...);

/**
 * 从文件描述符读取数据到缓冲区中。
 *
 * @param file  文件描述符，要读取数据的文件。
 * @param ptr   指向保存读取数据的缓冲区的指针。
 * @param len   要读取的字节数。
 *
 * @return      成功时返回实际读取的字节数，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_read(int file, char* ptr, int len);

/**
 * 将数据从缓冲区写入文件。
 *
 * @param file  文件描述符，要写入数据的文件。
 * @param ptr   指向包含要写入文件的数据的缓冲区的指针。
 * @param len   要写入的字节数。
 *
 * @return      成功时返回实际写入的字节数，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_write(int file, char* ptr, int len);

/**
 * 移动文件读/写指针到指定位置。
 *
 * @param file  文件描述符，要移动指针的文件。
 * @param ptr   相对于dir的偏移量，用于指定移动的位置。
 * @param dir   指针移动的方向。可以是SEEK_SET（从文件开头开始偏移），SEEK_CUR（从当前指针位置偏移），或者SEEK_END（从文件末尾开始偏移）。
 *
 * @return      成功时返回新的文件指针位置（以字节为单位），如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_lseek(int file, int ptr, int dir);

/**
 * 关闭一个文件描述符。
 *
 * @param file  要关闭的文件描述符。
 *
 * @return      成功时返回0，如果出现错误，返回-1，并设置errno来指示错误类型。
 */
int sys_close(int file);

#endif