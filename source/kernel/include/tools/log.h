#ifndef LOG_H
#define LOG_H

/**
 * @brief : 日志系统初始化
 * @return {*}
 */
void log_init(void);

/**
 * @brief: 输出日志
 * @param {char*} fmt 格式化字符串
 * @return {*}
 */
void log_printf(const char* fmt, ...);

#endif