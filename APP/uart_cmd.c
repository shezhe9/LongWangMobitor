/*
 * uart_cmd.c
 *
 *  Created on: 2025-10-10
 *      Author: AI Assistant
 *  Description: UART命令接收和处理（支持长字节累积接收）
 */

#include "CONFIG.h"
#include "uart_cmd.h"
#include "key.h"
#include "central.h"
#include "ulog_buffer.h"
#include "cmd_parsing.h"

//=============================================================================
// UART任务相关
//=============================================================================
uint8_t tUart_Task = 0xFF;
#define RE_READ_UART_EVENT  0x0001

//=============================================================================
// 串口接收缓冲区
//=============================================================================
uint8_t uart_rx_buffer[50];  // 接收缓冲区（容量大于20，防止溢出）
uint8_t rx_count = 0;        // 累计接收计数
uint8_t len_rcv = 0;
BOOL rcv_tout = FALSE;       // 超时标志
uint8_t trigB;

//=============================================================================
// UART任务事件处理函数
//=============================================================================
/**
 * @brief UART任务事件处理函数
 * 
 * 该函数在接收到UART数据后被TMOS调度执行
 * 主要功能：
 *  1. 读取剩余的UART数据
 *  2. 将完整的20字节命令复制到uart_parsing_buffer
 *  3. 触发CMD_RCV_UART_EVENT事件交给cmd_parsing任务处理
 */
static uint16_t Uart_Task_Event(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)
    {
        uint8_t *pMsg;
        if((pMsg = tmos_msg_receive(tUart_Task)) != NULL)
        {
            tmos_msg_deallocate(pMsg);
        }
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & RE_READ_UART_EVENT)
    {
        // 继续读取可能的剩余数据
        len_rcv = UART1_RecvString(&uart_rx_buffer[rx_count]);
        
        if(len_rcv == 0)
        {
            utrace("No more UART data, total received: %d bytes\n", rx_count);
        }
        else
        {
            utrace("Read additional %d bytes, total: %d bytes\n", len_rcv, rx_count + len_rcv);
            rx_count += len_rcv;
        }
        
        // 检查是否已接收到足够的数据（至少1字节，最多CMD_LEN字节）
        if(rx_count > 0)
        {
            // 将数据复制到cmd_parsing任务的uart_parsing_buffer
            if(uart_parsing_buffer_LOCK == FALSE)
            {
                uart_parsing_buffer_LOCK = TRUE;
                tmos_memset(uart_parsing_buffer, 0, sizeof(uart_parsing_buffer));
                
                // 复制实际接收到的字节数（最多CMD_LEN字节）
                uint8_t copy_len = (rx_count > CMD_LEN) ? CMD_LEN : rx_count;
                tmos_memcpy(uart_parsing_buffer, uart_rx_buffer, copy_len);
                
                // 触发命令解析事件
                tmos_set_event(tCmd_Parsing_Task, CMD_RCV_UART_EVENT);
                
                utrace("Copied %d bytes to parsing buffer\n", copy_len);
            }
            else
            {
                uwarn("uart_parsing_buffer is locked, data discarded!\n");
            }
        }
            
        // 重置接收计数器和缓冲区
        rx_count = 0;
        tmos_memset(uart_rx_buffer, 0, sizeof(uart_rx_buffer));

        return (events ^ RE_READ_UART_EVENT);
    }

    return 0;
}

/**
 * @brief 初始化UART任务
 */
void uart_task_init(void)
{
    tUart_Task = TMOS_ProcessEventRegister(Uart_Task_Event);
    if (tUart_Task != 0xFF)
    {
        uinfo("tUart_Task registered successfully, Task ID=%d\n", tUart_Task);
    }
    else
    {
        uinfo("ERROR: tUart_Task registration failed!\n");
    }
}

/**
 * @brief 初始化UART硬件配置
 */
void app_uart_init(void)
{
    // 使能串口接收中断
    PRINT("UART1_INTCfg \n");
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
    
    // 启用UART1的FIFO
    PRINT("UART1_FIFOCfg \n");
    R8_UART1_FCR |= RB_FCR_FIFO_EN;
    UART1_ByteTrigCfg(UART_7BYTE_TRIG);  // 触发阈值：7字节
    trigB = 7;
    
    // 清空FIFO中可能的残留数据
    while(R8_UART1_RFC > 0)
    {
        R8_UART1_RBR;
    }
    
    // 启用UART中断
    PRINT("PFIC_EnableIRQ \n");
    PFIC_EnableIRQ(UART1_IRQn);
}

//=============================================================================
// UART1中断服务函数
//=============================================================================
/**
 * @brief UART1中断服务函数
 * 
 * 累积接收机制：
 *  1. 每次中断接收FIFO中的所有数据
 *  2. 累加到rx_count
 *  3. 设置超时事件（100ms），等待更多数据
 *  4. 如果超时或接收够CMD_LEN字节，则触发处理事件
 */
__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
    uint16_t error;
    uint8_t i, temp_count;
    uint8_t errorcode = 0;
    
    switch(UART1_GetITFlag())
    {
        case UART_II_LINE_STAT:
            errorcode = UART1_GetLinSTA();
            uwarn("UART_II_LINE_STAT: 0x%02X\n", errorcode);
            if((errorcode & STA_ERR_FIFOOV) == STA_ERR_FIFOOV)
            {
                uwarn("UART FIFO overflow!\n");
            }
            break;

        case UART_II_RECV_RDY:  // 接收到7字节数据（达到触发阈值）
            len_rcv = UART1_RecvString(&uart_rx_buffer[rx_count]);
            utrace("UART_II_RECV_RDY: received %d bytes\n", len_rcv);
            
            // 启动100ms延时任务，等待可能的后续数据
            tmos_start_task(tUart_Task, RE_READ_UART_EVENT, MS1_TO_SYSTEM_TIME(100));
            rx_count += len_rcv;
            break;

        case UART_II_RECV_TOUT:  // 接收超时（FIFO有数据但未达到触发阈值）
            len_rcv = UART1_RecvString(&uart_rx_buffer[rx_count]);
            utrace("UART_II_RECV_TOUT: received %d bytes\n", len_rcv);
            
            // 停止之前的延时任务，立即触发处理
            tmos_stop_task(tUart_Task, RE_READ_UART_EVENT);
            tmos_set_event(tUart_Task, RE_READ_UART_EVENT);
            rx_count += len_rcv;
            break;

        case UART_II_THR_EMPTY:
            utrace("UART_II_THR_EMPTY\n");
            break;
            
        case UART_II_MODEM_CHG:
            utrace("UART_II_MODEM_CHG\n");
            break;
            
        default:
            break;
    }
}
