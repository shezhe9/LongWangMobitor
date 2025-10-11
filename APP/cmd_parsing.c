/*
 * cmd_parsing.c
 *
 *  Created on: 2025-10-10
 *      Author: AI Assistant
 *  Description: 命令解析系统实现
 */

#include "cmd_parsing.h"
#include "CONFIG.h"
#include "uart_cmd.h"
#include "key.h"
#include "central.h"
#include "ulog_buffer.h"

//=============================================================================
// 全局变量定义
//=============================================================================
// 解析缓冲区锁
BOOL uart_parsing_buffer_LOCK = FALSE;
BOOL ble_parsing_buffer_LOCK = FALSE;

// 解析缓冲区
uint8_t uart_parsing_buffer[CMD_LEN] = {0};
uint8_t ble_parsing_buffer[CMD_LEN] = {0};

// 命令解析缓冲区
uint8_t cmd_parsing_buffer[CMD_LEN] = {0};
uint8_t report_buffer[CMD_LEN] = {0};

// 命令解析任务ID
uint8_t tCmd_Parsing_Task = 0xFF;

//=============================================================================
// 命令解析核心函数
//=============================================================================
/**
 * @brief 命令解析核心函数（三回调机制）
 * 
 * 根据cmd_buf[2]的值决定执行流程：
 *  0 - 读取：只调用report_cb返回当前状态
 *  1 - 执行：只调用parsing_cb执行命令
 *  2 - 解析：调用parsing_cb解析并执行
 *  3 - 解析+保存：先调用parsing_cb，再调用save_cb
 */
void parsing_cmd(int8_t *cmd_buf, report_callback report_cb, 
                parsing_callback parsing_cb, save_callback save_cb)
{
    // 检查操作类型是否有效
    if (cmd_buf[2] > 3)
    {
        uinfo("Invalid operation type: 0x%X\n", cmd_buf[2]);
        return;
    }

    switch(cmd_buf[2])
    {
        case 0:  // 读取
            udebug("Operation: Read\n");
            if (report_cb != NULL)
            {
                report_cb(cmd_buf, CMD_LEN);
            }
            break;

        case 1:  // 仅执行
            udebug("Operation: Execute\n");
            if (parsing_cb != NULL)
            {
                parsing_cb(cmd_buf, CMD_LEN);
            }
            break;

        case 2:  // 解析执行
            udebug("Operation: Parse & Execute\n");
            if (parsing_cb != NULL)
            {
                parsing_cb(cmd_buf, CMD_LEN);
            }
            break;

        case 3:  // 解析执行并保存
            udebug("Operation: Parse & Execute & Save\n");
            if (parsing_cb != NULL)
            {
                parsing_cb(cmd_buf, CMD_LEN);
            }
            if (save_cb != NULL)
            {
                udebug("Saving to storage...\n");
                save_cb(cmd_buf, CMD_LEN);
            }
            break;
    }
}

//=============================================================================
// 命令处理函数实现
//=============================================================================

// 系统复位处理函数
void reset_sys_handler(uint8_t *data, uint16_t len)
{
    uinfo("Executing system reset...\n");
    
    // 看门狗复位使能
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RST_WDOG_CTRL = RB_WDOG_RST_EN;
    
    // 软件复位使能位
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
    
    while(1);  // 等待复位发生
}

// 模拟单击处理函数
void single_click_handler(uint8_t *data, uint16_t len)
{
    uinfo("Simulating single click event...\n");
    tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
}

// 发送测试数据处理函数
void send_test_data_handler(uint8_t *data, uint16_t len)
{
    uinfo("Starting to send test data via BLE...\n");
    uinfo("Triggering START_SEND_TEST_DATA_EVT (0x%04X) for centralTaskId=%d\n", 
          START_SEND_TEST_DATA_EVT, centralTaskId);
    // 立即触发事件，不需要延时
    tmos_set_event(centralTaskId, START_SEND_TEST_DATA_EVT);
}

// 断开连接处理函数
void disconnect_handler(uint8_t *data, uint16_t len)
{
    uinfo("Disconnecting BLE and stopping auto-reconnect...\n");
    Central_DisconnectAndStopAutoReconnect();
}

// 重新连接处理函数
void reconnect_handler(uint8_t *data, uint16_t len)
{
    uinfo("Starting auto-reconnect...\n");
    Central_StartAutoReconnect();
}

// 开始扫描处理函数
void start_scan_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Start scanning\n");
    // TODO: 实现扫描功能
}

// 停止扫描处理函数
void stop_scan_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Stop scanning\n");
    // TODO: 实现停止扫描功能
}

// 连接设备处理函数
void connect_device_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Connect to device\n");
    // TODO: 实现连接功能
    ulog_array_to_hex("Device address:", &data[3], 6);
}

// 发送数据处理函数
void send_data_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Send data to peripheral\n");
    ulog_array_to_hex("Data to send:", data, len);
    // TODO: 实现数据发送功能
}

// 设置扫描参数处理函数
void set_scan_param_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Set scan parameters\n");
    // TODO: 实现设置扫描参数功能
}

// 转发到从机处理函数
void forward_to_peripheral_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Forward to peripheral\n");
    ulog_array_to_hex("Forward data:", data, len);
    // TODO: 实现转发功能
}

//=============================================================================
// 协议解析入口函数
//=============================================================================
/**
 * @brief 协议解析入口 - 根据命令类型分发处理
 */
void protocol_parsing(uint8_t *buf, uint16_t len)
{
    // 复制命令到解析缓冲区
    tmos_memcpy(cmd_parsing_buffer, buf, len);
    
    // 打印接收到的命令
    ulog_array_to_hex("Received command:", cmd_parsing_buffer, len);
    
    // 根据命令类型分发处理
    switch(cmd_parsing_buffer[0])
    {
        // 系统控制类命令
        case MSG_SINGLE_CLICK_TEST:
            uinfo("Parsing: MSG_SINGLE_CLICK_TEST\n");
            parsing_cmd(cmd_parsing_buffer, NULL, single_click_handler, NULL);
            break;

        case MSG_SEND_TEST_DATA:
            uinfo("Parsing: MSG_SEND_TEST_DATA\n");
            // 对于发送测试数据命令，忽略操作类型，直接执行
            send_test_data_handler(cmd_parsing_buffer, CMD_LEN);
            break;

        case MSG_DISCONNECT:
            uinfo("Parsing: MSG_DISCONNECT\n");
            parsing_cmd(cmd_parsing_buffer, NULL, disconnect_handler, NULL);
            break;

        case MSG_RECONNECT:
            uinfo("Parsing: MSG_RECONNECT\n");
            parsing_cmd(cmd_parsing_buffer, NULL, reconnect_handler, NULL);
            break;

        case MSG_RESET_SYS:
            uinfo("Parsing: MSG_RESET_SYS\n");
            parsing_cmd(cmd_parsing_buffer, NULL, reset_sys_handler, NULL);
            break;

        // 扫描控制类命令
        case MSG_START_SCAN:
            uinfo("Parsing: MSG_START_SCAN\n");
            parsing_cmd(cmd_parsing_buffer, NULL, start_scan_handler, NULL);
            break;

        case MSG_STOP_SCAN:
            uinfo("Parsing: MSG_STOP_SCAN\n");
            parsing_cmd(cmd_parsing_buffer, NULL, stop_scan_handler, NULL);
            break;

        case MSG_SCAN_STATUS:
            uinfo("Parsing: MSG_SCAN_STATUS\n");
            uinfo("Command: Query scan status\n");
            // TODO: 实现查询扫描状态
            break;

        // 连接管理类命令
        case MSG_CONNECT_DEVICE:
            uinfo("Parsing: MSG_CONNECT_DEVICE\n");
            parsing_cmd(cmd_parsing_buffer, NULL, connect_device_handler, NULL);
            break;

        case MSG_DISCONNECT_DEVICE:
            uinfo("Parsing: MSG_DISCONNECT_DEVICE\n");
            uinfo("Command: Disconnect device\n");
            // TODO: 实现断开设备功能
            break;

        case MSG_CONNECTION_STATUS:
            uinfo("Parsing: MSG_CONNECTION_STATUS\n");
            uinfo("Command: Query connection status\n");
            // TODO: 实现查询连接状态
            break;

        // 数据传输类命令
        case MSG_SEND_DATA:
            uinfo("Parsing: MSG_SEND_DATA\n");
            parsing_cmd(cmd_parsing_buffer, NULL, send_data_handler, NULL);
            break;

        case MSG_READ_DATA:
            uinfo("Parsing: MSG_READ_DATA\n");
            uinfo("Command: Read data from peripheral\n");
            // TODO: 实现读取数据功能
            break;

        // 配置类命令
        case MSG_SET_SCAN_PARAM:
            uinfo("Parsing: MSG_SET_SCAN_PARAM\n");
            parsing_cmd(cmd_parsing_buffer, NULL, set_scan_param_handler, NULL);
            break;

        case MSG_SET_CONN_PARAM:
            uinfo("Parsing: MSG_SET_CONN_PARAM\n");
            uinfo("Command: Set connection parameters\n");
            // TODO: 实现设置连接参数
            break;

        case MSG_GET_DEVICE_INFO:
            uinfo("Parsing: MSG_GET_DEVICE_INFO\n");
            uinfo("Command: Get device information\n");
            // TODO: 实现获取设备信息
            break;

        // 转发类命令
        case MSG_FWD_TO_PERIPHERAL:
            uinfo("Parsing: MSG_FWD_TO_PERIPHERAL\n");
            parsing_cmd(cmd_parsing_buffer, NULL, forward_to_peripheral_handler, NULL);
            break;

        // 未知命令
        default:
            uinfo("Unknown command type: 0x%02X\n", cmd_parsing_buffer[0]);
            ulog_array_to_hex("Unknown command (HEX):", cmd_parsing_buffer, CMD_LEN);
            ulog_array_to_dec("Unknown command (DEC):", cmd_parsing_buffer, CMD_LEN);
            break;
    }
}

//=============================================================================
// TMOS 任务事件处理
//=============================================================================
/**
 * @brief 命令解析任务事件处理函数
 */
static uint16_t Cmd_Parsing_Task_Event(uint8_t task_id, uint16_t events)
{
    // 系统消息事件
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;
        if((pMsg = tmos_msg_receive(tCmd_Parsing_Task)) != NULL)
        {
            tmos_msg_deallocate(pMsg);
        }
        return (events ^ SYS_EVENT_MSG);
    }

    // UART命令接收事件
    if(events & CMD_RCV_UART_EVENT)
    {
        utrace("Processing CMD_RCV_UART_EVENT\n");
        if(uart_parsing_buffer_LOCK == TRUE)
        {
            protocol_parsing(uart_parsing_buffer, CMD_LEN);
            uart_parsing_buffer_LOCK = FALSE;
        }
        return (events ^ CMD_RCV_UART_EVENT);
    }

    // BLE命令接收事件
    if(events & CMD_RCV_BLE_EVENT)
    {
        utrace("Processing CMD_RCV_BLE_EVENT\n");
        if(ble_parsing_buffer_LOCK == TRUE)
        {
            protocol_parsing(ble_parsing_buffer, CMD_LEN);
            ble_parsing_buffer_LOCK = FALSE;
        }
        return (events ^ CMD_RCV_BLE_EVENT);
    }

    // 返回未处理的事件
    return 0;
}

/**
 * @brief 命令解析任务初始化
 */
void cmd_parsing_task_init(void)
{
    // 注册命令解析任务
    tCmd_Parsing_Task = TMOS_ProcessEventRegister(Cmd_Parsing_Task_Event);
    
    if (tCmd_Parsing_Task != 0xFF)
    {
        uinfo("tCmd_Parsing_Task registered successfully, Task ID=%d\n", tCmd_Parsing_Task);
    }
    else
    {
        uinfo("ERROR: tCmd_Parsing_Task registration failed!\n");
    }
}

