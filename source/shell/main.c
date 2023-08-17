/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-17 21:40:10
 * @Description:
 */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "lib_syscall.h"

static cli_t cli;
static const char* promot = "sh >>";

/**
 * help命令
 */
static int do_help(int argc, char** argv) {
    return 0;
}

// 命令列表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .usage = "help -- list support command",
        .do_func = do_help,
    },
};

/**
 * 显示命令行提示符
 */
static void show_promot(void) {
    printf("%s", cli.promot);
    fflush(stdout);
}

/**
 * 命令行初始化
 */
static void cli_init(const char* promot, int cnt) {
    cli.promot = promot;
    // 清空输入缓冲区
    memset(cli.curr_input, 0, CLI_INPUT_SIZE);

    cli.cmd_start = cmd_list;
    cli.cmd_end = cmd_list + cnt;
}

int main(int argc, char** argv) {
    open(argv[0], 0);
    dup(0);
    dup(0);

    printf("Welcome \033[33;49mLumosOS\n");
    printf("\033[39;49m---------------\n");
    printf("VERSION: %s\n", OS_VERSION);
    printf("Author: ChenXr\n");
    printf("Create Data: 2023-07\n");

    cli_init(promot, sizeof(cmd_list) / sizeof(cmd_list[0]));
    for (;;) {
        show_promot();
        gets(cli.curr_input);
    }
}