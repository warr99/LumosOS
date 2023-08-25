/*
 * @Author: warrior
 * @Date: 2023-08-07 13:56:41
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-25 23:37:14
 * @Description:
 */
#include "main.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include "fs/file.h"
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
        printf("%s\n", start->usage);
        start++;
    }
    return 0;
}

static int do_clear(int argc, char** argv) {
    printf("%s", ESC_CLEAR_SCREEN);
    printf("%s", ESC_MOVE_CURSOR(0, 0));
    return 0;
}

static int do_echo(int argc, char** argv) {
    // 只有一个参数(echo) -> 等待用户输入
    if (argc == 1) {
        char msg_buf[128];
        fgets(msg_buf, sizeof(msg_buf), stdin);
        msg_buf[sizeof(msg_buf) - 1] = '\0';
        puts(msg_buf);
        return 0;
    }
    int count = 1;
    int ch;
    while ((ch = getopt(argc, argv, "n:h")) != -1) {
        switch (ch) {
            case 'h':
                puts("echo echo any message");
                puts("Usage: echo [-n count] msg");
                optind = 1;  // getopt需要多次调用，需要重置
                return 0;
            case 'n':
                count = atoi(optarg);
                break;
            case '?':
                if (optarg) {
                    fprintf(stderr, "Unknown option: -%s\n", optarg);
                }
                optind = 1;  // getopt需要多次调用，需要重置
                return -1;
            default:
                break;
        }
    }
    // 索引已经超过了最后一个参数的位置，意味着没有传入要发送的信息
    if (optind > argc - 1) {
        fprintf(stderr, "Message is empty \n");
        optind = 1;  // getopt需要多次调用，需要重置
        return -1;
    }
    // 循环打印消息
    char* msg = argv[optind];
    for (int i = 0; i < count; i++) {
        puts(msg);
    }
    optind = 1;  // getopt需要多次调用，需要重置
    return 0;
}

static int do_exit(int argc, char** argv) {
    exit(0);
    return 0;
}

static int do_ls(int argc, char** argv) {
    // 打开目录
    DIR* p_dir = opendir("temp");
    if (p_dir == NULL) {
        printf("open dir failed\n");
        return -1;
    }
    struct dirent* entry;
    while ((entry = readdir(p_dir)) != NULL) {
        strlwr(entry->name);
        printf("%c %s %d\n",
               entry->type == FILE_DIR ? 'd' : 'f',
               entry->name,
               entry->size);
    }
    closedir(p_dir);

    return 0;
}

/**
 * @brief 列出文本文件内容
 */
static int do_less(int argc, char** argv) {
    int ch;
    while ((ch = getopt(argc, argv, "lh")) != -1) {
        switch (ch) {
            case 'h':
                puts("show file content");
                puts("less [-l] file");
                puts("-l show file line by line.");
                break;
            case '?':
                if (optarg) {
                    fprintf(stderr, "Unknown option: -%s\n", optarg);
                }
                optind = 1;  // getopt需要多次调用，需要重置
                return -1;
        }
    }

    // 索引已经超过了最后一个参数的位置，意味着没有传入要发送的信息
    if (optind > argc - 1) {
        fprintf(stderr, "no file\n");
        optind = 1;  // getopt需要多次调用，需要重置
        return -1;
    }

    FILE* file = fopen(argv[optind], "r");
    if (file == NULL) {
        fprintf(stderr, "open file failed. %s", argv[optind]);
        optind = 1;  // getopt需要多次调用，需要重置
        return -1;
    }

    char* buf = (char*)malloc(255);

    while (fgets(buf, 255, file) != NULL) {
        fputs(buf, stdout);
    }
    free(buf);
    fclose(file);
    optind = 1;  // getopt需要多次调用，需要重置
    return 0;
}

// 命令列表
static const cli_cmd_t cmd_list[] = {
    {
        .name = "help",
        .usage = "help -- list support command",
        .do_func = do_help,
    },
    {
        .name = "clear",
        .usage = "clear -- clear screen",
        .do_func = do_clear,
    },
    {
        .name = "echo",
        .usage = "echo [-n count] msg -- echo something",
        .do_func = do_echo,
    },
    {
        .name = "quit",
        .usage = "quit -- quit from shell",
        .do_func = do_exit,
    },
    {
        .name = "ls",
        .usage = "ls -- list director",
        .do_func = do_ls,
    },
    {
        .name = "less",
        .usage = "less [-l] file -- show file",
        .do_func = do_less,
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
        fprintf(stderr, ESC_COLOR_ERROR "error: %d\n" ESC_COLOR_DEFAULT, ret);
    }
}

static void run_exec_file(const char* path, int argc, char** argv) {
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed: %s", path);
    } else if (pid == 0) {
        // 以下供测试exit使用
        for (int i = 0; i < argc; i++) {
            msleep(1000);
            printf("arg %d = %s\n", i, argv[i]);
        }
        exit(-1);
    } else {
        // 等待子进程执行完毕
        int status;
        int pid = wait(&status);
        fprintf(stderr, "cmd %s result: %d, pid = %d\n", path, status, pid);
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
    open(argv[0], O_RDWR);
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
        run_exec_file("", argc, argv);
        // 找不到命令，提示错误
        fprintf(stderr, ESC_COLOR_ERROR "Unknown command: %s\n" ESC_COLOR_DEFAULT, cli.curr_input);
    }
    return 0;
}