/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : SPI0��ʾ Master/Slave ģʽ�����շ�
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

__attribute__((aligned(4))) uint8_t grb_scale[LED_NUM*3] = {0};     //ÿ�������G/R/B��ɫ�Ҷȸ���һ���ֽڱ�ʾ
__attribute__((aligned(4))) uint8_t spi_grb_buff[LED_NUM*3*4];     //ÿ�������ÿ����ɫ����4�ֽڱ�ʾ��




/*
 * ��GRB�Ҷ�ת��Ϊͨ��SPI��DMA�����ı�������ÿ��WS2812������Ҫ12���ֽ���������
 * uint8_t *grb_pdest:  SPI��������
 * uint8_t *grb_psrc:   GRB�Ҷ�
 * uint16_t len:        ��������
 * */
void grb_scale_2_spi_buff(uint8_t *pdest_grb, uint8_t *psrc_grb, uint16_t len)
{
    memset(pdest_grb, 0, len*3*4);     //��������
//  G/R/B�����е�һ������λ��4��SPIλ��ʾ��ÿ��ɫ��8������λ����Ҫ4�ֽڡ�����G/R/B��24��λ������ҪSPI�����е�96��λ��12���ֽ�����ʾ��
    for(uint16_t i=0; i<len; i++)
    {
        for(uint8_t j=0; j<3; j++)  //G/R/B����ɫ����������
        {
            for(uint8_t k=0; k<4; k++)  //ÿ��ɫ��8λ�Ҷ�,��4���ֽڱ�ʾ
            {
                for(uint8_t m=0; m<2; m++)  //ÿ���ֽڰ�������ws2812����λ
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
// ������ɫ


void display(uint8_t G_set,uint8_t R_set,uint8_t B_set)
{
    uint8_t i;
    uint8_t R=0;
    uint8_t G=0;
    uint8_t B=0;

    memset(spi_grb_buff, 0, 24);     //ģ��WS2812�ĸ�λ�ź�
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
        DelayMs(delay); // ���Ʊ仯�ٶ�
        PRINT(" RGB =%d %d %d \n",R,G,B);
    }
    memset(spi_grb_buff, 0, 24);     //ģ��WS2812�ĸ�λ�ź�
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
        DelayMs(delay); // ���Ʊ仯�ٶ�
        PRINT(" RGB =%d %d %d \n",R,G,B);
    }
};



void setDimColor(uint32_t color, float brightness) {
    // ��ջ�����
    memset(spi_grb_buff, 0, 24); // ģ�� WS2812 �ĸ�λ�ź�

    // ��ȡ RGB ��ɫͨ������������
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    // �������ȵ�����ɫ
    uint8_t dimR = r * brightness;
    uint8_t dimG = g * brightness;
    uint8_t dimB = b * brightness;

    // ������ɫ�� LED �ƴ�
    for (uint8_t i = 0; i < LED_NUM; i++) {
        grb_scale[3 * i]     = dimG; // G
        grb_scale[3 * i + 1] = dimR; // R
        grb_scale[3 * i + 2] = dimB; // B
    }

    // ����ɫ���ݷ��͵� LED �ƴ�
    grb_scale_2_spi_buff(spi_grb_buff, grb_scale, LED_NUM);
    SPI0_MasterDMATrans(spi_grb_buff, LED_NUM * 3 * 4);
}

//��SPI0ӳ�䵽PB12��PB13
void SPI0_PinRemap_Init(void) {
    // ʹ�ܰ�ȫ���ʣ��Ա��޸�����Ĵ���
    R8_SAFE_ACCESS_SIG = 0x55;
    R8_SAFE_ACCESS_SIG = 0xAA;

    // ��ȡ��ǰ SPI0 ӳ�����
    uint8_t current_mapping = (R16_PIN_ALTERNATE & (1 << 8)) >> 8;

    // ��ӡ��ǰ SPI0 ӳ�����
    if (current_mapping) {
        PRINT("old SPI0 map to PB12/PB13/PB14/PB15\n");
    } else {
        PRINT("old SPI0 map to PA12/PA13/PA14/PA15\n");
    }

    // �� SPI0 ����ӳ�䵽 PB��
    R16_PIN_ALTERNATE |= (1 << 8);  // �޸�Ϊ��8λ

    // ��ȡ��ǰӳ���������ӡ
    current_mapping = (R16_PIN_ALTERNATE & (1 << 8)) >> 8;
    if (current_mapping) {
        PRINT("now SPI0 map to PB12/PB13/PB14/PB15\n");
    } else {
        PRINT("now SPI0 map to PA12/PA13/PA14/PA15\n");
    }

    // �ָ���ȫ����״̬
    R8_SAFE_ACCESS_SIG = 0x55;
    R8_SAFE_ACCESS_SIG = 0xAA;

    // ֻ����MOSI���ţ�PB14��Ϊ�������
    GPIOB_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);  // MOSI
    PRINT("SPI0 MOSI pin configured on PB14\n");

    //masterģʽ�£�MOSI��Ĭ�ϵ�ƽ�ܵ�MISOӰ�죬���������������WS2812��
    //��Ҫ��MISO���ղ�������Ϊ�͵�ƽ���

    // ����MISO����Ϊ�͵�ƽ���
    GPIOB_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // MISO����Ϊ�������
    GPIOB_ResetBits(GPIO_Pin_15);  // MISO����Ϊ�͵�ƽ
}

float temper_env, humidity_env;
void ws2812_ini(void)
{
    //HSECFG_Capacitance(HSECap_18p);
    // ֻ����MOSI���ţ�PB14��Ϊ�������
    GPIOA_ModeCfg(GPIO_Pin_14, GPIO_ModeOut_PP_5mA);  // MOSI
    PRINT("SPI0 MOSI pin configured on PA14\n");
    //masterģʽ�£�MOSI��Ĭ�ϵ�ƽ�ܵ�MISOӰ�죬���������������WS2812��
    //��Ҫ��MISO���ղ�������Ϊ�͵�ƽ���
    GPIOA_ModeCfg(GPIO_Pin_15, GPIO_ModeOut_PP_5mA);  // MISO����Ϊ�������
    GPIOA_ResetBits(GPIO_Pin_15);  // MISO����Ϊ�͵�ƽ
    SPI0_MasterDefInit();
    SPI0_CLKCfg(19);    //SPI��ʱ�ӣ�62.4M��Ƶ��3.284MƵ�ʣ�ÿ4��bitģ��һ��ws2812����λ��ģ��Ŀ��Ϊ800KƵ�ʣ�
    memset(spi_grb_buff, 0, 24);     //ģ��WS2812�ĸ�λ�ź�
    SPI0_MasterDMATrans(spi_grb_buff, 24);
    //display(255,0,0);
    setDimColor(INI_STATE, 0.01); // 10% ����
}
