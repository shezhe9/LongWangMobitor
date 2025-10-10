/*
 * cmd_parsing.h
 *
 *  Created on: 2025-10-10
 *      Author: AI Assistant
 *  Description: 命令解析系统头文件
 */

#ifndef INCLUDE_CMD_PARSING_H_
#define INCLUDE_CMD_PARSING_H_

#include "CONFIG.h"
#include "common_define.h"

//=============================================================================
// TMOS 事件定义
//=============================================================================
#define CMD_RCV_UART_EVENT     0x0004  // UART接收命令事件
#define CMD_RCV_BLE_EVENT      0x0002  // BLE接收命令事件

//=============================================================================
// 命令解析缓冲区 - 全局变量
//=============================================================================
// UART解析缓冲区
extern BOOL uart_parsing_buffer_LOCK;
extern uint8_t uart_parsing_buffer[CMD_LEN];

// BLE解析缓冲区
extern BOOL ble_parsing_buffer_LOCK;
extern uint8_t ble_parsing_buffer[CMD_LEN];

// 命令解析任务ID
extern uint8_t tCmd_Parsing_Task;

// 当前解析缓冲区和回复缓冲区
extern uint8_t cmd_parsing_buffer[CMD_LEN];
extern uint8_t report_buffer[CMD_LEN];

//=============================================================================
// 回调函数类型定义
//=============================================================================
typedef void (*report_callback)(uint8_t *data, uint16_t len);   // 读取命令回调
typedef void (*parsing_callback)(uint8_t *data, uint16_t len);  // 解析执行回调
typedef void (*save_callback)(uint8_t *data, uint16_t len);     // 保存回调

//=============================================================================
// 命令类型枚举
//=============================================================================
enum {
    // 系统控制类 (0x38 - 0x6F)
    MSG_SINGLE_CLICK_TEST = 0x38,    // 模拟单击事件
    MSG_SEND_TEST_DATA = 0x3A,       // 发送测试数据
    MSG_DISCONNECT = 0x3B,           // 断开BLE连接
    MSG_RECONNECT = 0x3C,            // 重新连接
    MSG_RESET_SYS = 0x6F,            // 系统复位

    // 扫描控制类 (0x40 - 0x42)
    MSG_START_SCAN = 0x40,           // 开始扫描
    MSG_STOP_SCAN = 0x41,            // 停止扫描
    MSG_SCAN_STATUS = 0x42,          // 查询扫描状态

    // 连接管理类 (0x50 - 0x52)
    MSG_CONNECT_DEVICE = 0x50,       // 连接指定设备
    MSG_DISCONNECT_DEVICE = 0x51,    // 断开指定设备
    MSG_CONNECTION_STATUS = 0x52,    // 查询连接状态

    // 数据传输类 (0x60 - 0x61)
    MSG_SEND_DATA = 0x60,            // 发送数据到从机
    MSG_READ_DATA = 0x61,            // 读取从机数据

    // 配置类 (0x70 - 0x72)
    MSG_SET_SCAN_PARAM = 0x70,       // 设置扫描参数
    MSG_SET_CONN_PARAM = 0x71,       // 设置连接参数
    MSG_GET_DEVICE_INFO = 0x72,      // 获取设备信息

    // 转发类 (0x80)
    MSG_FWD_TO_PERIPHERAL = 0x80,    // 转发命令到从机

    // 未知命令
    NO_MSG = 0xFF,
};

//=============================================================================
// 核心函数声明
//=============================================================================
/**
 * @brief 命令解析任务初始化
 */
extern void cmd_parsing_task_init(void);

/**
 * @brief 协议解析入口函数
 * @param buf 命令缓冲区
 * @param len 命令长度
 */
extern void protocol_parsing(uint8_t *buf, uint16_t len);

/**
 * @brief 命令解析核心函数（三回调机制）
 * @param cmd_buf 命令缓冲区
 * @param report_cb 读取回调（操作类型=0时调用）
 * @param parsing_cb 解析执行回调（操作类型=1/2/3时调用）
 * @param save_cb 保存回调（操作类型=3时调用）
 * 
 * 操作类型说明：
 *  0 - 读取：只调用report_cb返回当前状态
 *  1 - 执行：只调用parsing_cb执行命令
 *  2 - 解析：调用parsing_cb解析并执行
 *  3 - 解析+保存：先调用parsing_cb，再调用save_cb
 */
extern void parsing_cmd(int8_t *cmd_buf, report_callback report_cb, 
                       parsing_callback parsing_cb, save_callback save_cb);

#endif /* INCLUDE_CMD_PARSING_H_ */
