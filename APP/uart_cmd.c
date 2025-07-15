#include "CONFIG.h"
#include "uart_cmd.h"
#include "ws2812.h"
#include "key.h"
#include "central.h"  // ���central.hͷ�ļ��Է���centralTaskId���¼�����


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
    //����UART1��fifo    (R8_UARTx_FCR)�� RB_FCR_FIFO_EN λ��1
    PRINT("UART1_FIFOCfg \n");
    R8_UART1_FCR |= RB_FCR_FIFO_EN;
    UART1_ByteTrigCfg(11);//11��7 �ֽڡ�
    
    PRINT("PFIC_EnableIRQ \n");
    PFIC_EnableIRQ(UART1_IRQn);
}

//
//Not every uart reception will end with a UART_II_RECV_TOUT
//UART_II_RECV_TOUT can only be triggered when R8_UARTx_RFC is not 0
//Here we cannot rely UART_II_RECV_TOUT as the end of a uart reception
//����20�ֽڻ��������ڽ�������
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
              // ֱ�Ӵ�����յ�������
            do
            {
                //d��ӡfifo�ļ�����ֵ
                //PRINT("count: %d\n", R8_UART1_RFC);
                //��R8_UART1_RFC�����ݴ洢��������
                for(uint8_t i = 0; i < R8_UART1_RFC; i++)   
                {
                    uart_rx_buffer[i] = R8_UART1_RBR;
                }

                // ���ִ�ӡ���յ����ַ���
                //for(uint8_t i = 0; i < R8_UART1_RFC; i++)
                //{
                //    PRINT("char: 0x%x\n", uart_rx_buffer[i]);
                //}
              
                
                // ��ӡ�׸����ݵ�ʮ������ֵ
                //PRINT("First: 0x%X\n", uart_rx_buffer[0]);
                //���uart_rx_buffer[0]=0x31 ������CH582Mϵͳ
                if (uart_rx_buffer[0] == 0x31)
                {
                    PRINT("EN_CH_SWITCH\n");
                    EN_CH_SWITCH();
                }

                if (uart_rx_buffer[0] == 0x32)
                {
                    PRINT("EN_ESP_SWITCH\n");
                    EN_ESP_SWITCH();
                }

                if (uart_rx_buffer[0] == 0x33)
                {
                    PRINT("EN_ESP_ME_SWITCH\n");
                    EN_ESP_ME_SWITCH();
                }
                if (uart_rx_buffer[0] == 0x34)
                {
                    PRINT("EN_ESP_UART1_LOG_SWITCH\n");
                    EN_ESP_UART1_LOG_SWITCH();
                }

                if (uart_rx_buffer[0] == 0x35)
                {
                    PRINT("EN_TEMP_SWITCH\n");
                    EN_TEMP_SWITCH();
                }

                if (uart_rx_buffer[0] == 0x36)
                {
                    
                }

                if (uart_rx_buffer[0] == 0x38)
                {
                    tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                }
               
                if (uart_rx_buffer[0] == 0x39)
                {
                    // ʹ�ܿ��Ź�
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
                    R8_RST_WDOG_CTRL = RB_WDOG_RST_EN;
                    // �������Ź���λ
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
                    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
                    R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
                    // �ȴ���λ����
                    while(1);
                }

                if (uart_rx_buffer[0] == 0x3a)
                {
                    PRINT("Received 0x3a command - Starting to send 20 test data to BLE device\n");
                    // ��������20�����������¼�
                    tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 10); // 10ms��ʼ����
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
