/********************************** (C) COPYRIGHT *******************************
 * File Name          : oled_display.h
 * Author             : Raditor
 * Version            : V1.0
 * Date               : 2024/10/08
 * Description        : OLED显示接口 - 提供温度等信息显示
 *******************************************************************************/

#ifndef __OLED_DISPLAY_H
#define __OLED_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/******************************************************************************
 * OLED功能总开关
 * 注释掉下面这行可以完全禁用OLED功能，节省代码空间
 ******************************************************************************/
#define ENABLE_OLED_DISPLAY

/******************************************************************************
 * OLED驱动芯片选择
 * 取消注释下面其中一行来选择使用的驱动芯片：
 * USE_SH1107 - 使用SH1107驱动 (64x128像素，竖屏)
 * USE_SH1106 - 使用SH1106驱动 (128x64像素，横屏)
 ******************************************************************************/
#ifdef ENABLE_OLED_DISPLAY
//#define USE_SH1107      // 使用SH1107驱动
 #define USE_SH1106   // 使用SH1106驱动
#endif

#ifdef ENABLE_OLED_DISPLAY

/**
 * @brief OLED显示初始化
 */
void OLED_Display_Init(void);

/**
 * @brief 更新温度显示
 * @param env_temp 环境温度 (单位: 0.1°C)
 * @param left_temp 左侧温度 (单位: 0.1°C)
 * @param water_temp 水温 (单位: 0.1°C)
 * @param right_temp 热端温度 (单位: 0.1°C)
 * @param cold_delta 降温值
 * @param mode_type 模式类型 (1-文,2-娱,3-专,4-联,7-冷,8-关,10-未,11-错)
 * @param cold_pwm_set 冷功率PWM设定值
 * @param fan_fix_speed 风扇速度
 * @param bump_fix_speed 水泵速度
 */
void OLED_Update_Temp_Display(int16_t env_temp, int16_t left_temp, int16_t water_temp, int16_t right_temp,
                               int16_t cold_delta, uint8_t mode_type, uint8_t cold_pwm_set,
                               uint8_t fan_fix_speed, uint8_t bump_fix_speed);

/**
 * @brief 清空OLED显示
 */
void OLED_Display_Clear(void);

#endif  // ENABLE_OLED_DISPLAY

#ifdef __cplusplus
}
#endif

#endif /* __OLED_DISPLAY_H */