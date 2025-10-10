#include "CONFIG.h"
#include "uart_cmd.h"
#include "key.h"
#include "central.h"  // 添加central.h头文件以访问centralTaskId和事件定义
#include "ulog_buffer.h"  // ulog 日志系统


void app_uart_init(void)
{

    //tx fifo and tx fifo
    //The buffer length should be a power of 2
    //app_drv_fifo_init(&app_uart_tx_fifo, app_uart_tx_buffer, APP_UART_TX_BUFFER_LENGTH);
    //app_drv_fifo_init(&app_uart_rx_fifo, app_uart_rx_buffer, APP_UART_RX_BUFFER_LENGTH);

    //uart tx io
   // GPIOA_SetBits(bTXD1);
   // GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);

    //uart rx io
    //GPIOA_SetBits(bRXD1);
    //GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU);

    //uart1 init
   // UART1_DefInit();

    //enable interupt
    PRINT("UART1_INTCfg \n");
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY | RB_IER_LINE_STAT);
    //起用UART1的fifo    (R8_UARTx_FCR)的 RB_FCR_FIFO_EN 位置1
    PRINT("UART1_FIFOCfg \n");
    R8_UART1_FCR |= RB_FCR_FIFO_EN;
    UART1_ByteTrigCfg(11);//11：7 字节。
    
    PRINT("PFIC_EnableIRQ \n");
    PFIC_EnableIRQ(UART1_IRQn);
}

//
//Not every uart reception will end with a UART_II_RECV_TOUT
//UART_II_RECV_TOUT can only be triggered when R8_UARTx_RFC is not 0
//Here we cannot rely UART_II_RECV_TOUT as the end of a uart reception
//定义20字节缓冲区用于接收数据
uint8_t uart_rx_buffer[20];

__INTERRUPT
__HIGH_CODE
void UART1_IRQHandler(void)
{
    uint16_t error;
    switch(UART1_GetITFlag())
    {
        case UART_II_LINE_STAT:
            UART1_GetLinSTA();
            break;

        case UART_II_RECV_RDY:
        case UART_II_RECV_TOUT:
              // 直接处理接收到的数据
            do
            {
                //d打印fifo的计数器值
                //uinfo("count: %d\n", R8_UART1_RFC);
                //将R8_UART1_RFC个数据存储到缓冲区
                for(uint8_t i = 0; i < R8_UART1_RFC; i++)   
                {
                    uart_rx_buffer[i] = R8_UART1_RBR;
                }

                // 逐字打印接收到的字符串
                //for(uint8_t i = 0; i < R8_UART1_RFC; i++)
                //{
                //    uinfo("char: 0x%x\n", uart_rx_buffer[i]);
                //}
              
                
                // 打印首个数据的十六进制值
                //uinfo("First: 0x%X\n", uart_rx_buffer[0]);
                
                // 串口命令处理
                if (uart_rx_buffer[0] == 0x38)
                {
                    tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                }
               
                if (uart_rx_buffer[0] == 0x39)
                {
                    // 使能看门狗
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
                    R8_RST_WDOG_CTRL = RB_WDOG_RST_EN;
                    // 触发看门狗复位
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
                    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
                    // 等待复位发生
                    while(1);
                }

                if (uart_rx_buffer[0] == 0x3a)
                {
                    uinfo("Recv 0x3a: Start send test data\n");
                    // 触发发送20个测试数据事件
                    tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 10); // 10ms后开始发送
                }

                if (uart_rx_buffer[0] == 0x3b)
                {
                    uinfo("Recv 0x3b: Disconnect and stop\n");
                    // 断开BLE连接并停止自动重连
                    Central_DisconnectAndStopAutoReconnect();
                }

                if (uart_rx_buffer[0] == 0x3c)
                {
                    uinfo("Recv 0x3c: Start reconnect\n");
                    // 启动自动搜索和连接
                    Central_StartAutoReconnect();
                }

            }while(R8_UART1_RFC > 0);
            //uart_rx_flag = true;
            break;

        case UART_II_THR_EMPTY:
            break;
        case UART_II_MODEM_CHG:
            break;
        default:
            break;
    }
}
