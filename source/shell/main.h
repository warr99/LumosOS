/*
 * @Author: warrior
 * @Date: 2023-08-17 20:50:02
 * @LastEditors: warrior
 * @LastEditTime: 2023-08-17 21:39:49
 * @Description:
 */
#ifndef MAIN_H
#define MAIN_H

#define CLI_INPUT_SIZE 1024

typedef struct _cli_cmd_t {
    const char* name;                       // 命令名称
    const char* usage ;                     // 使用方法
    int (*do_func)(int argc, char** argv);  // 对应函数
} cli_cmd_t;

typedef struct _cli_t {
    char curr_input[CLI_INPUT_SIZE];  // 当前输入缓存
    const cli_cmd_t* cmd_start;       // 命令起始
    const cli_cmd_t* cmd_end;         // 命令起始
    const char* promot;               // 提示符
} cli_t;

#endif