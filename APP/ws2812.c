/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : SPI0演示 Master/Slave 模式数据收发
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH58x_common.h"
#include "ws2812.h"

__attribute__((aligned(4))) UINT8 spiBuff[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
__attribute__((aligned(4))) UINT8 spiBuffrev[16];


#define LED_NUM 1

#define GRB_CODE_0 (0x80)
#define GRB_CODE_1 (0xE0)

__attribute__((aligned(4))) uint8_t grb_scale[LED_NUM*3] = {0};     //每个灯珠的G/R/B三色灰度各用一个字节表示
__attribute__((aligned(4))) uint8_t spi_grb_buff[LED_NUM*3*4];     //每个灯珠的每种颜色，用4字节表示。




/*
 * 将GRB灰度转换为通过SPI的DMA发出的比特流。每个WS2812灯珠需要12个字节来驱动。
 * uint8_t *grb_pdest:  SPI缓存数组
 * uint8_t *grb_psrc:   GRB灰度
 * uint16_t len:        灯珠数量
 * */
void grb_scale_2_spi_buff(uint8_t *pdest_grb, uint8_t *psrc_grb, uint16_t len)
{
    memset(pdest_grb, 0, len*3*4);     //数组清零
//  G/R/B驱动中的一个编码位用4个SPI位表示。每种色彩8个编码位，需要4字节。驱动G/R/B共24个位，故需要SPI波形中的96个位即12个字节来表示。
    for(uint16_t i=0; i<len; i++)
    {
        for(uint8_t j=0; j<3; j++)  //G/R/B三种色彩轮流配置
        {
            for(uint8_t k=0; k<4; k++)  //每种色彩8位灰度,用4个字节表示
            {
                for(uint8_t m=0; m<2; m++)  //每个字节包含两个ws2812编码位
                {
                    if( psrc_grb[3*i + j] & (0x80 >> (2*k + m)) )
                    {
                        pdest_grb[3*4*i + 4*j +k] |= (GRB_CODE_1 >> (m*4));
                        //PRINT("1 ");
                    }
                    else
                    {
                        pdest_grb[3*4*i + 4*j +k] |= (GRB_CODE_0 >> (m*4));
                        //PRINT("0 ");
                    }
                }
                //PRINT("%x ", spi_grb_buff[3*4*i + 4*j +k]);
            }
        }
        //PRINT("\n");
    }
    //PRINT("\n");
}
#define MAX_NUM 10
#define delay 100
// 定义颜色


void display(uint8_t G_set,uint8_t R_set,uint8_t B_set)
{
    uint8_t i;
    uint8_t R=0;
    uint8_t G=0;
    uint8_t B=0;

    memset(spi_grb_buff, 0, 24);     //模拟WS2812的复位信号
    for(uint8_t brightness = 0; brightness <= MAX_NUM; brightness++)
    {

        if(R_set==0)
            R=0;
        else
            R=brightness;
        if(G_set==0)
            G=0;
        else
            G=brightness;
        if(B_set==0)
            B=0;
        else
            B=brightness;
        grb_scale[3*i + 0] = G; // G
        grb_scale[3*i + 1] = R;       // R
        grb_scale[3*i + 2] = B;       // B
        grb_scale_2_spi_buff(spi_grb_buff, grb_scale, LED_NUM);
        SPI0_MasterDMATrans(spi_grb_buff, LED_NUM*3*4);
        DelayMs(delay); // 控制变化速度
        PRINT(" RGB =%d %d %d \n",R,G,B);
    }
    memset(spi_grb_buff, 0, 24);     //模拟WS2812的复位信号
    for(uint8_t brightness = MAX_NUM; brightness > 0; brightness--)
    {
        if(R_set==0)
            R=0;
        else
            R=brightness;
        if(G_set==0)
            G=0;
        else
            G=brightness;
        if(B_set==0)
            B=0;
        else
            B=brightness;
        grb_scale[3*i + 0] = G; // G
        grb_scale[3*i + 1] = R;       // R
        grb_scale[3*i + 2] = B;       // B
        grb_scale_2_spi_buff(spi_grb_buff, grb_scale, LED_NUM);
        SPI0_MasterDMATrans(spi_grb_buff, LED_NUM*3*4);
        DelayMs(delay); // 控制变化速度
        PRINT(" RGB =%d %d %d \n",R,G,B);
    }
};



void setDimColor(uint32_t color, float brightness) {
    // 清空缓冲区
    memset(spi_grb_buff, 0, 24); // 模拟 WS2812 的复位信号

    // 提取 RGB 颜色通道并调整亮度
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // 根据亮度调整颜色
    uint8_t dimR = r * brightness;
    uint8_t dimG = g * brightness;
    uint8_t dimB = b * brightness;

    // 设置颜色到 LED 灯带
    for (uint8_t i = 0; i < LED_NUM; i++) {
        grb_scale[3 * i]     = dimG; // G
        grb_scale[3 * i + 1] = dimR; // R
        grb_scale[3 * i + 2] = dimB; // B
    }

    // 将颜色数据发送到 LED 灯带
    grb_scale_2_spi_buff(spi_grb_buff, grb_scale, LED_NUM);
    SPI0_MasterDMATrans(spi_grb_buff, LED_NUM * 3 * 4);
}

//将SPI0映射到PB12和PB13
void SPI0_PinRemap_Init(void) {
    // 使能安全访问，以便修改特殊寄存器
    R8_SAFE_ACCESS_SIG = 0x55;
    R8_SAFE_ACCESS_SIG = 0xAA;

    // 获取当前 SPI0 映射情况
    uint8_t current_mapping = (R16_PIN_ALTERNATE & (1 << 8)) >> 8;

    // 打印当前 SPI0 映射情况
    if (current_mapping) {
        PRINT("old SPI0 map to PB12/PB13/PB14/PB15\n");
    } else {
        PRINT("old SPI0 map to PA12/PA13/PA14/PA15\n");
    }

    // 将 SPI0 引脚映射到 PB组
    R16_PIN_ALTERNATE |= (1 << 8);  // 修改为第8位

    // 获取当前映射情况并打印
    current_mapping = (R16_PIN_ALTERNATE & (1 << 8)) >> 8;
    if (current_mapping) {
        PRINT("now SPI0 map to PB12/PB13/PB14/PB15\n");
    } else {
        PRINT("now SPI0 map to PA12/PA13/PA14/PA15\n");
    }

    // 恢复安全访问状态
    R8_SAFE_ACCESS_SIG = 0x55;
    R8_SAFE_ACCESS_SIG = 0xAA;

    // 只配置MOSI引脚（PB14）为推挽输出
    GPIOB_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);  // MOSI
    PRINT("SPI0 MOSI pin configured on PB14\n");

    //master模式下，MOSI的默认电平受到MISO影响，如果用来驱动诸如WS2812，
    //需要把MISO悬空并且设置为低电平输出

    // 配置MISO引脚为低电平输出
    GPIOB_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // MISO设置为推挽输出
    GPIOB_ResetBits(GPIO_Pin_15);  // MISO设置为低电平
}

float temper_env, humidity_env;
void ws2812_ini(void)
{
    //HSECFG_Capacitance(HSECap_18p);
    // 只配置MOSI引脚（PB14）为推挽输出
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);  // MOSI
    PRINT("SPI0 MOSI pin configured on PA14\n");
    //master模式下，MOSI的默认电平受到MISO影响，如果用来驱动诸如WS2812，
    //需要把MISO悬空并且设置为低电平输出
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // MISO设置为推挽输出
    GPIOA_ResetBits(GPIO_Pin_15);  // MISO设置为低电平
    SPI0_MasterDefInit();
    SPI0_CLKCfg(19);    //SPI的时钟，62.4M分频到3.284M频率，每4个bit模拟一个ws2812编码位（模拟目标为800K频率）
    memset(spi_grb_buff, 0, 24);     //模拟WS2812的复位信号
    SPI0_MasterDMATrans(spi_grb_buff, 24);
    //display(255,0,0);
    setDimColor(INI_STATE, 0.01); // 10% 亮度
}
