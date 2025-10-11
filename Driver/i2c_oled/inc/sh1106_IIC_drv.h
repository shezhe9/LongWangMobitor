/********************************** (C) COPYRIGHT *******************************
 * File Name          : sh1106_IIC_drv.h
 * Author             : Raditor
 * Version            : V1.0
 * Date               : 2024/10/08
 * Description        : SH1106 OLED driver header - 使用硬件IIC
 *******************************************************************************/

#ifndef __SH1106_IIC_DRV_H
#define __SH1106_IIC_DRV_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// SH1106 OLED显示器配置
#define SH1106_ADDR         0x3C    // SH1106 IIC地址（7位）
#define SH1106_WIDTH        128     // 显示宽度
#define SH1106_HEIGHT       64      // 显示高度

#define SH1106_CMD          0       // 写命令
#define SH1106_DATA         1       // 写数据

// OLED显存缓冲区
extern uint8_t OLED_GRAM_1106[SH1106_WIDTH][8];

// 基础函数
void SH1106_Init(void);                                 // OLED初始化
void SH1106_Clear(void);                                // 清屏
void SH1106_Refresh(void);                              // 刷新显示
void SH1106_Display_On(void);                           // 打开显示
void SH1106_Display_Off(void);                          // 关闭显示

// 绘图函数
void SH1106_DrawPoint(uint8_t x, uint8_t y, uint8_t t); // 画点
void SH1106_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode);  // 显示字符
void SH1106_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size, uint8_t mode); // 显示字符串
void SH1106_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size, uint8_t mode); // 显示数字
void SH1106_ShowChinese(uint8_t x, uint8_t y, uint8_t index, uint8_t mode); // 显示中文（16x16）
void SH1106_ShowNum16(uint8_t x, uint8_t y, int32_t num, uint8_t mode); // 显示数字（16x16，不显示前导0）
void SH1106_ShowColon16(uint8_t x, uint8_t y, uint8_t mode); // 显示冒号（16x16）
void SH1106_ShowColon8(uint8_t x, uint8_t y, uint8_t mode); // 显示冒号（8x16，紧凑）
void SH1106_ShowSpace8(uint8_t x, uint8_t y, uint8_t mode); // 显示空格（8x16）
void SH1106_ShowSpace4(uint8_t x, uint8_t y, uint8_t mode); // 显示空格（4x16，精简）
void SH1106_ShowPercent8(uint8_t x, uint8_t y, uint8_t mode); // 显示百分号（8x16）

#ifdef __cplusplus
}
#endif

#endif /* __SH1106_IIC_DRV_H */
