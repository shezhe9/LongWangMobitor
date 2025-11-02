#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ulog_buffer.h"
#include "CONFIG.h"

// TMOS 定时延迟定义 (单位: 0.625ms)
// 注意：TMOS 的时间单位是 0.625ms，所以 10ms = 16 ticks
#ifndef DELAY_10MS
#define DELAY_10MS 16
#endif

// 日志处理任务ID
static uint8_t tPrint_Log_Task = 0xFF;

// 日志缓冲区
static ulog_buffer_t g_ulog_buffer;

/*********************************************************************
 * @fn      Print_Log_ProcessEvent
 *
 * @brief   日志处理任务事件处理函数
 *
 * @param   task_id - 任务ID
 * @param   events - 事件标志
 *
 * @return  未处理的事件
 */
static uint16_t Print_Log_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & PRINT_LOG_EVENT)
    {
        // 处理日志缓冲区中的日志，每次最多处理20条
        uint8_t count = 0;
        uint8_t max_logs = 20;
        
        while(count < max_logs && ulog_buffer_has_logs())
        {
            ulog_buffer_process_one();
            count++;
        }
        
        // 10ms 后再次处理日志
        tmos_start_task(tPrint_Log_Task, PRINT_LOG_EVENT, DELAY_10MS);  
        
        return (events ^ PRINT_LOG_EVENT);
    }
    
    // 返回未处理的事件
    return 0;
}

// 初始化日志缓冲区和日志处理任务
void ulog_buffer_init(void)
{
    // 初始化缓冲区
    memset(&g_ulog_buffer, 0, sizeof(g_ulog_buffer));
    
    // 注册日志处理任务
    tPrint_Log_Task = TMOS_ProcessEventRegister(Print_Log_ProcessEvent);
    PRINT("ulog 日志处理任务 ID: %d\n", tPrint_Log_Task);  // 打印任务注册
    
    // 立即触发一次任务，让它开始处理日志并设置周期性调度
    // 注意：在初始化阶段 tmos_start_task 可能失败，所以使用 tmos_set_event
    tmos_set_event(tPrint_Log_Task, PRINT_LOG_EVENT);
}

// 在中断中调用的日志函数
void ulog_buffer_log(uint8_t level, const char *fmt, ...)
{
    // 如果缓冲区已满，丢弃此条日志
    if(g_ulog_buffer.count >= ULOG_MAX_ENTRIES)
        return;
    
    // 获取当前写入位置
    ulog_buffer_entry_t *entry = &g_ulog_buffer.entries[g_ulog_buffer.write_index];
    
    // 设置日志级别和时间戳
    entry->level = level;
    entry->timestamp = R32_RTC_CNT_32K; // 使用RTC计数作为时间戳
    
    // 格式化日志消息
    va_list args;
    va_start(args, fmt);
    vsnprintf(entry->message, ULOG_MAX_ENTRY_SIZE, fmt, args);
    va_end(args);
    
    // 更新写入索引
    g_ulog_buffer.write_index = (g_ulog_buffer.write_index + 1) % ULOG_MAX_ENTRIES;
    g_ulog_buffer.count++;
}

// 检查是否有日志需要处理
uint8_t ulog_buffer_has_logs(void)
{
    return g_ulog_buffer.count > 0;
}

// 处理一条日志
void ulog_buffer_process_one(void)
{
    if(g_ulog_buffer.count > 0)
    {
        ulog_buffer_entry_t *entry = &g_ulog_buffer.entries[g_ulog_buffer.read_index];
        
        // 将RTC ticks转换为秒.毫秒格式
        // RTC频率 = 32768 Hz, 1 tick = 1/32768 秒 ≈ 0.0305ms
        // 总毫秒 = timestamp * 1000 / 32768 ≈ timestamp * 125 / 4096 (使用整数运算避免浮点)
        uint32_t total_ms = (entry->timestamp * 125) / 4096;  // 更精确的整数运算
        uint32_t seconds = total_ms / 1000;
        uint32_t milliseconds = total_ms % 1000;
        
        // 打印时间戳和日志级别 (格式: [秒.毫秒][级别])
        PRINT("[%lu.%03lu][", seconds, milliseconds);
        
        // 打印日志级别
        switch(entry->level)
        {
            case ULOG_LEVEL_TRACE: PRINT("T"); break;
            case ULOG_LEVEL_DEBUG: PRINT("D"); break;
            case ULOG_LEVEL_INFO:  PRINT("I"); break;
            case ULOG_LEVEL_WARN:  PRINT("W"); break;
            case ULOG_LEVEL_ERROR: PRINT("E"); break;
            case ULOG_LEVEL_FATAL: PRINT("F"); break;
            default: PRINT("?"); break;
        }
        
        // 打印日志内容
        PRINT("] %s", entry->message);
        
        // 更新读取索引
        g_ulog_buffer.read_index = (g_ulog_buffer.read_index + 1) % ULOG_MAX_ENTRIES;
        g_ulog_buffer.count--;
    }
}

//打印数组内容 16进制显示
//predef 前缀提示文 必须以"0"结尾
//buf 要打印的数组
//len 数组长度
void ulog_array_to_hex(uint8_t *predef,uint8_t *buf, uint8_t len)
{
    char str[3*len+1];
    for (int i = 0; i < len; i++) {
        sprintf(&str[i*3], "%02X ", buf[i]);
    }
    str[3*len] = '\0';
    uinfo("%s:%s \n",predef,str);
}

//打印数组内容 10进制显示
//predef 前缀提示文 必须以"0"结尾
//buf 要打印的数组
//len 数组长度
void ulog_array_to_dec(uint8_t *predef,uint8_t *buf, uint8_t len)
{
    char str[3*len+1];
    for (int i = 0; i < len; i++) {
        sprintf(&str[i*3], "%02d ", buf[i]);
    }
    str[3*len] = '\0';
    uinfo("%s:%s \n",predef,str);
}

