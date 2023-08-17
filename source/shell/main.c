/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-18 00:11:48
 * @Description:
 */
#include "main.h"
#include <stdio.h>
#include <string.h>
#include "lib_syscall.h"

static cli_t cli;
static const char* promot = "sh>>";

/**
 * help命令
 */
static int do_help(int argc, char** argv) {
    const cli_cmd_t* start = cli.cmd_start;
    // 循环打印名称及用法
    while (start < cli.cmd_end) {
        printf("%s -- %s\n", start->name, start->usage);
        start++;
    }
    return 0;
}

static int do_clear(int argc, char** argv) {
    printf("%s", ESC_CLEAR_SCREEN);
    printf("%s", ESC_MOVE_CURSOR(0, 0));
    return 0;
}

// 命令列表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .usage = "list support command",
        .do_func = do_help,
    },
    {
        .name = "clear",
        .usage = "clear screen",
        .do_func = do_clear,
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
 * @brief 查找内部命令
 * @param  name 内部命令名称
 * @return cli_cmd_t
 */
static const cli_cmd_t* find_builtin(const char* name) {
    for (const cli_cmd_t* cmd = cli.cmd_start; cmd < cli.cmd_end; cmd++) {
        if (strcmp(cmd->name, name) != 0) {
            continue;
        }

        return cmd;
    }

    return (const cli_cmd_t*)0;
}

/**
 * @brief 运行内部命令
 * @param  cmd 命令
 * @param  argc 参数个数
 * @param  argv 参数列表
 */
static void run_builtin(const cli_cmd_t* cmd, int argc, char** argv) {
    int ret = cmd->do_func(argc, argv);
    if (ret < 0) {
        fprintf(stderr,ESC_COLOR_ERROR"error: %d\n"ESC_COLOR_DEFAULT, ret);
    }
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
        char* str = fgets(cli.curr_input, CLI_INPUT_SIZE, stdin);
        if (!str) {
            continue;
        }
        char* cr = strchr(cli.curr_input, '\n');
        if (cr) {
            *cr = '\0';
        }
        cr = strchr(cli.curr_input, '\r');
        if (cr) {
            *cr = '\0';
        }
        int argc = 0;
        char* argv[CLI_MAX_ARG_COUNT];
        memset(argv, 0, sizeof(argv));
        const char* space = " ";  // 字符分割器
        char* token = strtok(cli.curr_input, space);
        while (token) {
            // 记录参数
            argv[argc++] = token;

            // 先获取下一位置
            token = strtok(NULL, space);
        }
        if (argc == 0) {
            continue;
        }
        // 尝试从内部命令表中加载内部命令
        const cli_cmd_t* cmd = find_builtin(argv[0]);
        if (cmd) {
            run_builtin(cmd, argc, argv);
            continue;
        }
        // 找不到命令，提示错误
        fprintf(stderr, ESC_COLOR_ERROR"Unknown command: %s\n"ESC_COLOR_DEFAULT, cli.curr_input);
    }
    return 0;
}