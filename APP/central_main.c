/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2020/08/06
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "hal.h"
#include "central.h"
#include "uart_cmd.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
const uint8_t MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   ��ѭ��
 *
 * @return  none
 */
__HIGH_CODE
__attribute__((noinline))
void Main_Circulation()
{
    while(1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   ������
 *
 * @return  none
 */
int main(void)
{
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    GPIOA_SetBits(bRXD1);//added by longwang
    GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU);//added by longwang
    UART1_DefInit();
#endif

    PRINT("%s\n", VER_LIB);
    app_uart_init();
    CH58X_BLEInit();
    HAL_Init();

    GAPRole_CentralInit();
    Central_Init();
    PRINT("ws2812_ini()\n");
    ws2812_ini();
    PRINT("Key_Init()\n");
    Key_Init();
        // ���� PB5 Ϊ���ģʽ����ʼ����Ϊ�͵�ƽ
    GPIOB_ModeCfg(GPIO_Pin_5, GPIO_ModeOut_PP_5mA); // ���� PB5 Ϊ�������
    GPIOB_ResetBits(GPIO_Pin_5); // Ĭ�ϵ͵�ƽ

    Main_Circulation();
}

/******************************** endfile @ main ******************************/
