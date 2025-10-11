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
/* 头锟侥硷拷锟斤拷锟斤拷 */
#include "CONFIG.h"
#include "hal.h"
#include "central.h"
#include "uart_cmd.h"
#include "key.h"
#include "ulog_buffer.h"  // ulog 日志系统
#include "cmd_parsing.h"  // 命令解析模块
#include "oled_display.h" // OLED显示
#include "app_i2c.h"      // I2C驱动

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
 * @brief   锟斤拷循锟斤拷
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
 * @brief   锟斤拷锟斤拷锟斤拷
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
    
    // 初始化 ulog 日志系统（必须在 GAPRole_CentralInit() 之后！）
    ulog_buffer_init();
    uinfo("ulog \310\325\326\276\317\265\315\263\263\365\312\274\273\257\315\352\263\311\n");  // 日志系统初始化完成

    Central_Init();
    
    // 使用 ulog 替代 PRINT
    uinfo("Key_Init() - \260\264\274\374\263\365\312\274\273\257\277\252\312\274\n");  // 按键初始化开始
    Key_Init();
    uinfo("Key_Init() - \260\264\274\374\263\365\312\274\273\257\315\352\263\311\n");  // 按键初始化完成
    
    // 初始化UART任务（TMOS任务注册）
    PRINT("Initializing uart_task...\n");
    uart_task_init();
    PRINT("uart_task initialized\n");
    
    // 初始化命令解析任务（TMOS任务注册）
    PRINT("Initializing cmd_parsing_task...\n");
    cmd_parsing_task_init();
    PRINT("cmd_parsing_task initialized\n");
    
#ifdef ENABLE_OLED_DISPLAY
    // 初始化I2C和OLED显示
    PRINT("Initializing I2C and OLED...\n");
    i2c_app_init(0x3C);  // 初始化I2C，0x3C是OLED地址
    OLED_Display_Init(); // 初始化OLED显示
    uinfo("OLED Display Initialized\n");
#endif

    Main_Circulation();
}

/******************************** endfile @ main ******************************/
