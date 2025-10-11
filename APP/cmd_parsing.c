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

// 发送控制命令处理函数（MSG_SEND_CONTROL_CMD 0x33）
void send_control_cmd_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Send control command to peripheral\n");
    
    // 解析命令参数
    uint8_t x1 = data[3];   // 模式控制（1-4）
    uint8_t CC = data[4];   // CC参数（0-3）
    uint8_t x2 = data[5];   // 制冷算法（1-3）
    uint8_t x3 = data[6];   // 算法具体参数值
    uint8_t x4 = data[7];   // 水泵具体值（0-100）
    uint8_t x5 = data[8];   // 风扇具体值（0-100）
    uint8_t x6 = data[9];   // x6参数
    uint8_t x7 = data[10];  // x7参数
    
    // 模式转换：1→0x81, 2→0x82, 3→0x83, 4→0x84
    uint8_t mode_byte = 0x80 + x1;
    
    uinfo("Mode: %d (0x%02X), CC: %d, Algo: %d, Value: %d, Pump: %d, Fan: %d\n",
          x1, mode_byte, CC, x2, x3, x4, x5);
    
    // 组装20字节发送包（按用户指定的格式）
    uint8_t send_buffer[20];
    send_buffer[0]  = mode_byte;  // x1 → 0x81-0x84
    send_buffer[1]  = 0x00;
    send_buffer[2]  = CC;
    send_buffer[3]  = x2;
    send_buffer[4]  = x3;
    send_buffer[5]  = 0x02;
    send_buffer[6]  = 0x00;
    send_buffer[7]  = x4;         // 水泵值
    send_buffer[8]  = 0x00;
    send_buffer[9]  = 0x00;
    send_buffer[10] = 0x02;
    send_buffer[11] = 0x00;
    send_buffer[12] = x5;         // 风扇值
    send_buffer[13] = 0x00;
    send_buffer[14] = 0x00;
    send_buffer[15] = x6;
    send_buffer[16] = 0x00;
    send_buffer[17] = x7;
    send_buffer[18] = 0x00;
    send_buffer[19] = 0x00;
    
    ulog_array_to_hex("Control command:", send_buffer, 20);
    
    // 检查连接状态
    if(centralState != BLE_STATE_CONNECTED || centralConnHandle == GAP_CONNHANDLE_INIT)
    {
        uinfo("Not connected, cannot send control command\n");
        return;
    }
    
    // 检查写特征句柄
    if(centralWriteCharHdl == 0)
    {
        uinfo("Write characteristic not found\n");
        return;
    }
    
    // 检查GATT操作状态
    if(centralProcedureInProgress == TRUE)
    {
        uinfo("GATT procedure in progress, cannot send command now\n");
        return;
    }
    
    // 发送控制命令到AE10
    uinfo("Sending control command (20 bytes) to AE10 (handle: 0x%04X)\n", centralWriteCharHdl);
    
    attWriteReq_t req;
    req.cmd = TRUE;
    req.sig = FALSE;
    req.handle = centralWriteCharHdl;
    req.len = 20;
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
    
    if(req.pValue != NULL)
    {
        tmos_memcpy(req.pValue, send_buffer, 20);
        
        bStatus_t status = GATT_WriteCharValue(centralConnHandle, &req, centralTaskId);
        if(status == SUCCESS)
        {
            uinfo("Control command sent successfully!\n");
        }
        else
        {
            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
            uinfo("Failed to send control command (status=0x%02X)\n", status);
        }
    }
    else
    {
        uinfo("Failed to allocate memory for control command\n");
    }
}

// 发送数据处理函数
void send_data_handler(uint8_t *data, uint16_t len)
{
    uinfo("Command: Send data to peripheral\n");
    
    // 数据从cmd_buf[3]开始，长度为17字节（CMD_LEN=20 - 前3字节）
    // 但需要填充到20字节发送
    uint8_t payload_buffer[20];  // 20字节缓冲区
    tmos_memcpy(payload_buffer, &data[3], CMD_LEN - 3);  // 复制17字节
    // 剩余3字节填充为0x00
    payload_buffer[17] = 0x00;
    payload_buffer[18] = 0x00;
    payload_buffer[19] = 0x00;
    
    ulog_array_to_hex("Data to send:", payload_buffer, 20);
    
    // 检查连接状态
    if(centralState != BLE_STATE_CONNECTED || centralConnHandle == GAP_CONNHANDLE_INIT)
    {
        uinfo("Not connected, cannot send data\n");
        return;
    }
    
    // 检查写特征句柄是否有效
    if(centralWriteCharHdl == 0)
    {
        uinfo("Write characteristic not found\n");
        return;
    }
    
    // 检查是否有其他GATT操作正在进行
    if(centralProcedureInProgress == TRUE)
    {
        uinfo("GATT procedure in progress, cannot send data now\n");
        return;
    }
    
    // 发送数据到AE10写特征（填充到20字节）
    uinfo("Sending 20 bytes to AE10 (handle: 0x%04X)\n", centralWriteCharHdl);
    
    attWriteReq_t req;
    req.cmd = TRUE;                                    // 使用Write Command（无需响应）
    req.sig = FALSE;                                   // 不带签名
    req.handle = centralWriteCharHdl;                  // AE10写特征句柄
    req.len = 20;                                      // 填充到20字节
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
    
    if(req.pValue != NULL)
    {
        // 复制填充后的20字节数据到GATT缓冲区
        tmos_memcpy(req.pValue, payload_buffer, 20);
        
        // 发送写命令
        bStatus_t status = GATT_WriteCharValue(centralConnHandle, &req, centralTaskId);
        if(status == SUCCESS)
        {
            uinfo("Data sent successfully!\n");
        }
        else
        {
            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
            uinfo("Failed to send data (status=0x%02X)\n", status);
        }
    }
    else
    {
        uinfo("Failed to allocate memory for data send\n");
    }
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
            // 统一使用回调机制，根据cmd_buf[2]判断操作类型
            parsing_cmd(cmd_parsing_buffer, NULL, send_test_data_handler, NULL);
            break;

        case MSG_DISCONNECT:
            uinfo("Parsing: MSG_DISCONNECT\n");
            parsing_cmd(cmd_parsing_buffer, NULL, disconnect_handler, NULL);
            break;

        case MSG_RECONNECT:
            uinfo("Parsing: MSG_RECONNECT\n");
            // 统一使用回调机制，根据cmd_buf[2]判断操作类型
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

        case MSG_SEND_CONTROL_CMD:
            uinfo("Parsing: MSG_SEND_CONTROL_CMD\n");
            parsing_cmd(cmd_parsing_buffer, NULL, send_control_cmd_handler, NULL);
            break;

        // 配置类命令（已从新编码规范中移除，暂时注释）
        // case MSG_SET_SCAN_PARAM:
        //     uinfo("Parsing: MSG_SET_SCAN_PARAM\n");
        //     parsing_cmd(cmd_parsing_buffer, NULL, set_scan_param_handler, NULL);
        //     break;

        // case MSG_SET_CONN_PARAM:
        //     uinfo("Parsing: MSG_SET_CONN_PARAM\n");
        //     uinfo("Command: Set connection parameters\n");
        //     // TODO: 实现设置连接参数
        //     break;

        // case MSG_GET_DEVICE_INFO:
        //     uinfo("Parsing: MSG_GET_DEVICE_INFO\n");
        //     uinfo("Command: Get device information\n");
        //     // TODO: 实现获取设备信息
        //     break;

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

