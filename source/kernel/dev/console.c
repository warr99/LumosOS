#include "dev/console.h"

static console_t console_buf[CONSOLE_NR];

int console_init(void) {
    for (int i = 0; i < CONSOLE_NR; i++) {
        console_t* console = console_buf + i;

        console->dispaly_cols = CONSOLE_COL_MAX;
        console->display_rows = CONSOLE_ROW_MAX;
        console->disp_base = (disp_char_t*)CONSOLE_DISP_ADDR + i * (CONSOLE_COL_MAX * CONSOLE_ROW_MAX);
    }
    return 0;
}

int console_write(int dev, char* data, int size) {
    console_t* console = console_buf + dev;

    int len;
    for (len = 0; len < size; len++) {
        char c = *data++;
        // TODO 写入
    }
    return len;
}

void console_close(int dev) {
}
