/*
 * ulog_buffer.h
 *
 *  Created on: Mar 23, 2025
 *      Author: Administrator
 */

#ifndef INCLUDE_ULOG_BUFFER_H_
#define INCLUDE_ULOG_BUFFER_H_

#include "CH58x_common.h"
#include "CONFIG.h"
#include "ulog_config.h"  // 包含日志配置文件


// 日志级别定义
#define ULOG_LEVEL_TRACE 0
#define ULOG_LEVEL_DEBUG 1
#define ULOG_LEVEL_INFO  2
#define ULOG_LEVEL_WARN  3
#define ULOG_LEVEL_ERROR 4
#define ULOG_LEVEL_FATAL 5
#define ULOG_LEVEL_NONE  6

// 缓冲区配置参数
#define ULOG_BUFFER_SIZE 4096  // 缓冲区大小，注意RAM限制
#define ULOG_MAX_ENTRY_SIZE 128  // 单条日志最大长度
#define ULOG_MAX_ENTRIES (ULOG_BUFFER_SIZE / ULOG_MAX_ENTRY_SIZE)  // 最大日志条数

// 事件定义
#define PRINT_LOG_EVENT       0x0001
#define PRINT_LOG_PERIOD      1600  // 日志1秒 (1600 * 0.625ms)

// 日志条目结构
typedef struct {
    uint8_t level;
    uint32_t timestamp;
    char message[ULOG_MAX_ENTRY_SIZE];
} ulog_buffer_entry_t;

// 日志缓冲区结构
typedef struct {
    ulog_buffer_entry_t entries[ULOG_MAX_ENTRIES];
    uint16_t write_index;
    uint16_t read_index;
    uint16_t count;
} ulog_buffer_t;

// 初始化日志缓冲区
void ulog_buffer_init(void);

// 添加日志到缓冲区
void ulog_buffer_log(uint8_t level, const char *fmt, ...);

// 检查是否有日志等待处理
uint8_t ulog_buffer_has_logs(void);

// 处理一条日志
void ulog_buffer_process_one(void);

// 处理所有待发送的日志
void ulog_buffer_process(void);

//数组转换为16进制字符串
void ulog_array_to_hex(uint8_t *predef,uint8_t *buf, uint8_t len);
//len 字节长度
void ulog_array_to_dec(uint8_t *predef,uint8_t *buf, uint8_t len);

// 日志宏定义
#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_TRACE
#define utrace(fmt, ...) ulog_buffer_log(ULOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#else
#define utrace(fmt, ...) ((void)0)
#endif

#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_DEBUG
#define udebug(fmt, ...) ulog_buffer_log(ULOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#else
#define udebug(fmt, ...) ((void)0)
#endif

#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_INFO
#define uinfo(fmt, ...)  ulog_buffer_log(ULOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#else
#define uinfo(fmt, ...)  ((void)0)
#endif

#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_WARN
#define uwarn(fmt, ...)  ulog_buffer_log(ULOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#else
#define uwarn(fmt, ...)  ((void)0)
#endif

#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_ERROR
#define uerror(fmt, ...) ulog_buffer_log(ULOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#else
#define uerror(fmt, ...) ((void)0)
#endif

#if ULOG_DEFAULT_LEVEL <= ULOG_LEVEL_FATAL
#define ufatal(fmt, ...) ulog_buffer_log(ULOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
#else
#define ufatal(fmt, ...) ((void)0)
#endif

// 日志缓冲区宏定义别名
#define ULOG_BUFFER_TRACE(fmt, ...) utrace(fmt, ##__VA_ARGS__)
#define ULOG_BUFFER_DEBUG(fmt, ...) udebug(fmt, ##__VA_ARGS__)
#define ULOG_BUFFER_INFO(fmt, ...)  uinfo(fmt, ##__VA_ARGS__)
#define ULOG_BUFFER_WARN(fmt, ...)  uwarn(fmt, ##__VA_ARGS__)
#define ULOG_BUFFER_ERROR(fmt, ...) uerror(fmt, ##__VA_ARGS__)
#define ULOG_BUFFER_FATAL(fmt, ...) ufatal(fmt, ##__VA_ARGS__)

#endif /* INCLUDE_ULOG_BUFFER_H_ */

