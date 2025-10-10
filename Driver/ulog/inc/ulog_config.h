/*
 * ulog_config.h
 *
 *  Created on: Oct 10, 2025
 *      Author: AI Assistant
 */

#ifndef INCLUDE_ULOG_CONFIG_H_
#define INCLUDE_ULOG_CONFIG_H_

// 定义默认日志级别
// ULOG_LEVEL_TRACE = 0  - 显示所有级别日志
// ULOG_LEVEL_DEBUG = 1  - 显示 DEBUG 及以上级别
// ULOG_LEVEL_INFO  = 2  - 显示 INFO 及以上级别（推荐生产环境）
// ULOG_LEVEL_WARN  = 3  - 显示 WARN 及以上级别
// ULOG_LEVEL_ERROR = 4  - 仅显示 ERROR 和 FATAL
// ULOG_LEVEL_FATAL = 5  - 仅显示 FATAL
// ULOG_LEVEL_NONE  = 6  - 关闭所有日志
#define ULOG_DEFAULT_LEVEL ULOG_LEVEL_TRACE

// 使能 ulog 功能（可选，也可以在编译参数中定义）
#define ULOG_ENABLED

#endif /* INCLUDE_ULOG_CONFIG_H_ */

