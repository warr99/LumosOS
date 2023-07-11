/*
 * @Author: warrior
 * @Date: 2023-07-11 11:27:21
 * @LastEditors: warrior
 * @LastEditTime: 2023-07-11 14:33:41
 * @Description: 工作在16位实模式下的c代码
 */
__asm__(".code16gcc");

static void show_msg(const char * msg) {
    char c;
	while ((c = *msg++) != '\0') {
		__asm__ __volatile__(
				"mov $0xe, %%ah\n\t"
				"mov %[ch], %%al\n\t"
				"int $0x10"::[ch]"r"(c));
	}
}
void loader_entry(void) {
    show_msg("......LumosOS Loading......\n\r");
    for (;;) {
    }
}
