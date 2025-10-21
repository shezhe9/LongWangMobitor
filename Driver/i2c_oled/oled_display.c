/********************************** (C) COPYRIGHT *******************************
 * File Name          : oled_display.c
 * Author             : Raditor
 * Version            : V1.0
 * Date               : 2024/10/08
 * Description        : OLED显示接口实现 - 支持SH1107和SH1106两种驱动
 *******************************************************************************/

#include "oled_display.h"

#ifdef ENABLE_OLED_DISPLAY

#include "CH58x_common.h"
#include "oledfont.h"  // 包含字库定义

// 根据宏定义包含对应的驱动头文件
#ifdef USE_SH1107
    #include "sh1107_i2c_drv.h"
#elif defined(USE_SH1106)
    #include "sh1106_IIC_drv.h"
#else
    #error "Please define USE_SH1107 or USE_SH1106 in oled_display.h"
#endif

/**
 * @brief OLED显示初始化
 */
void OLED_Display_Init(void)
{
#ifdef USE_SH1107
    SH1107_Init();
    SH1107_Clear();
    // 显示标题
    SH1107_ShowString(0, 0, (uint8_t *)"Temp Monitor", 8, 1);
    SH1107_Refresh();
#elif defined(USE_SH1106)
    SH1106_Init();
    SH1106_Clear();
    // 显示标题
    SH1106_ShowString(0, 0, (uint8_t *)"Temp Monitor", 8, 1);
    SH1106_Refresh();
#endif
}

/**
 * @brief 清空OLED显示
 */
void OLED_Display_Clear(void)
{
#ifdef USE_SH1107
    SH1107_Clear();
#elif defined(USE_SH1106)
    SH1106_Clear();
#endif
}

/**
 * @brief 更新温度显示
 * 显示格式：
 * 第1行: ev:25 (环境温度)
 * 第2行: lf:25 (左侧温度)
 * 第3行: wt:25 (水温)
 * 第4行: rt:25 (热端温度)
 */
void OLED_Update_Temp_Display(int16_t env_temp, int16_t left_temp, int16_t water_temp, int16_t right_temp,
                               int16_t cold_delta, uint8_t mode_type, uint8_t cold_pwm_set,
                               uint8_t fan_fix_speed, uint8_t bump_fix_speed, uint8_t conn_status, uint16_t device_version)
{
    int16_t temp_int;
    int16_t set_temp;
    uint8_t mode_char_index;
    uint8_t i, n;
    
#ifdef USE_SH1107
    // 清除显示区域（保留标题）
    for(i = 1; i < 16; i++)  // 从第2页开始清除
    {
        for(n = 0; n < SH1107_WIDTH; n++)
        {
            OLED_GRAM[n][i] = 0;
        }
    }
    
    // 环境温度 - 第2行 (y=10)
    SH1107_ShowString(0, 10, (uint8_t *)"ev:", 8, 1);
    temp_int = env_temp / 10;  // 转换为整数度
    if(temp_int >= -99 && temp_int <= 999)
    {
        SH1107_ShowNum(18, 10, temp_int, 3, 8, 1);
    }
    else
    {
        SH1107_ShowString(18, 10, (uint8_t *)"---", 8, 1);
    }
    
    // 左侧温度 - 第3行 (y=20)
    SH1107_ShowString(0, 20, (uint8_t *)"lf:", 8, 1);
    temp_int = left_temp / 10;
    if(temp_int >= -99 && temp_int <= 999)
    {
        SH1107_ShowNum(18, 20, temp_int, 3, 8, 1);
    }
    else
    {
        SH1107_ShowString(18, 20, (uint8_t *)"---", 8, 1);
    }
    
    // 水温 - 第4行 (y=30)
    SH1107_ShowString(0, 30, (uint8_t *)"wt:", 8, 1);
    temp_int = water_temp / 10;
    if(temp_int >= -99 && temp_int <= 999)
    {
        SH1107_ShowNum(18, 30, temp_int, 3, 8, 1);
    }
    else
    {
        SH1107_ShowString(18, 30, (uint8_t *)"---", 8, 1);
    }
    
    // 热端温度 - 第5行 (y=40)
    SH1107_ShowString(0, 40, (uint8_t *)"rt:", 8, 1);
    temp_int = right_temp / 10;
    if(temp_int >= -99 && temp_int <= 999)
    {
        SH1107_ShowNum(18, 40, temp_int, 3, 8, 1);
    }
    else
    {
        SH1107_ShowString(18, 40, (uint8_t *)"---", 8, 1);
    }
    
    // 刷新显示
    SH1107_Refresh();
    
#elif defined(USE_SH1106)
    // 清除整个显示区域
    for(i = 0; i < 8; i++)
    {
        for(n = 0; n < SH1106_WIDTH; n++)
        {
            OLED_GRAM_1106[n][i] = 0;
        }
    }
    
    /* 新布局：
     * 行1(y=0):  左:36  功:DD% 降:xx
     * 行2(y=16): 右:36  泵:FF% 设:BB
     * 行3(y=32): 室:36  风:EE%
     * 行4(y=48): 水:36             模:CC
     */
    
    // === 第1行 (y=0) ===
    // 左:36 或 左-36 (x=0) - 使用4像素精简字符
    SH1106_ShowChinese(0, 0, CHINESE_LEFT, 1);
    temp_int = left_temp / 10;
    if(temp_int >= -999 && temp_int <= 999)
    {
        if(temp_int >= 0)
        {
            SH1106_ShowColon4(16, 0, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(20, 0, temp_int, 1);  // 数字从x=20开始
        }
        else
        {
            SH1106_ShowMinus4(16, 0, 1);  // 负数显示4px负号
            SH1106_ShowNum16(20, 0, -temp_int, 1);  // 显示绝对值，负号已显示
        }
    }
    else
    {
        SH1106_ShowColon4(16, 0, 1);
        SH1106_ShowString(20, 0, (uint8_t *)"--", 16, 1);
    }
    
    // 功:DD% (x=42) - 使用4像素精简冒号
    SH1106_ShowSpace8(36, 0, 1);
    SH1106_ShowChinese(42, 0, CHINESE_POWER, 1);  // x=42, 16px
    SH1106_ShowColon4(58, 0, 1);  // x=58, 4px精简冒号
    if(cold_pwm_set <= 999)
    {
        SH1106_ShowNum16(62, 0, cold_pwm_set, 1);  // x=62
        SH1106_ShowPercent8(78, 0, 1);   // x=78, % (8px，结束于x=86)
        SH1106_ShowSpace4(86, 0, 1);     // x=86, 4px空格（结束于x=90）
    }
    else
    {
        SH1106_ShowString(62, 0, (uint8_t *)"--", 16, 1);
    }
    
    // 降:xx 或 降-xx (x=90) - 使用4像素精简字符
    SH1106_ShowChinese(90, 0, CHINESE_DOWN, 1);
    if(cold_delta >= -999 && cold_delta <= 999)
    {
        if(cold_delta >= 0)
        {
            SH1106_ShowColon4(106, 0, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(110, 0, cold_delta, 1);
        }
        else
        {
            SH1106_ShowMinus4(106, 0, 1);  // 负数显示4px负号
            SH1106_ShowNum16(110, 0, -cold_delta, 1);  // 显示绝对值
        }
    }
    else
    {
        SH1106_ShowColon4(106, 0, 1);
        SH1106_ShowString(110, 0, (uint8_t *)"--", 16, 1);
    }
    
    // === 第2行 (y=16) ===
    // 右:36 或 右-36 (x=0) - 使用4像素精简字符
    SH1106_ShowChinese(0, 16, CHINESE_RIGHT, 1);
    temp_int = right_temp / 10;
    if(temp_int >= -999 && temp_int <= 999)
    {
        if(temp_int >= 0)
        {
            SH1106_ShowColon4(16, 16, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(20, 16, temp_int, 1);  // 数字从x=20开始
        }
        else
        {
            SH1106_ShowMinus4(16, 16, 1);  // 负数显示4px负号
            SH1106_ShowNum16(20, 16, -temp_int, 1);  // 显示绝对值
        }
    }
    else
    {
        SH1106_ShowColon4(16, 16, 1);
        SH1106_ShowString(20, 16, (uint8_t *)"--", 16, 1);
    }
    
    // 泵:FF% (x=42) - 使用4像素精简冒号
    SH1106_ShowSpace8(36, 16, 1);
    SH1106_ShowChinese(42, 16, CHINESE_PUMP, 1);  // x=42, 16px
    SH1106_ShowColon4(58, 16, 1);  // x=58, 4px精简冒号
    if(bump_fix_speed <= 999)
    {
        SH1106_ShowNum16(62, 16, bump_fix_speed, 1);  // x=62
        SH1106_ShowPercent8(78, 16, 1);   // x=78, % (8px，结束于x=86)
        SH1106_ShowSpace4(86, 16, 1);     // x=86, 4px空格（结束于x=90）
    }
    else
    {
        SH1106_ShowString(62, 16, (uint8_t *)"--", 16, 1);
    }
    
    // 设:BB (x=90) - 使用4像素精简字符
    SH1106_ShowChinese(90, 16, CHINESE_SET, 1);
    set_temp = env_temp / 10 - cold_delta;
    if(set_temp >= -999 && set_temp <= 999)
    {
        if(set_temp >= 0)
        {
            SH1106_ShowColon4(106, 16, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(110, 16, set_temp, 1);  // 数字从x=110开始
        }
        else
        {
            SH1106_ShowMinus4(106, 16, 1);  // 负数显示4px负号
            SH1106_ShowNum16(110, 16, -set_temp, 1);  // 显示绝对值
        }
    }
    else
    {
        SH1106_ShowColon4(106, 16, 1);
        SH1106_ShowString(110, 16, (uint8_t *)"--", 16, 1);
    }
    
    // === 第3行 (y=32) ===
    // 室:36 或 室-36 (x=0) - 使用4像素精简字符
    SH1106_ShowChinese(0, 32, CHINESE_ROOM, 1);
    temp_int = env_temp / 10;
    if(temp_int >= -999 && temp_int <= 999)
    {
        if(temp_int >= 0)
        {
            SH1106_ShowColon4(16, 32, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(20, 32, temp_int, 1);  // 数字从x=20开始
        }
        else
        {
            SH1106_ShowMinus4(16, 32, 1);  // 负数显示4px负号
            SH1106_ShowNum16(20, 32, -temp_int, 1);  // 显示绝对值
        }
    }
    else
    {
        SH1106_ShowColon4(16, 32, 1);
        SH1106_ShowString(20, 32, (uint8_t *)"--", 16, 1);
    }
    
    // 风:EE% (x=42) - 使用4像素精简冒号
    SH1106_ShowSpace8(36, 32, 1);
    SH1106_ShowChinese(42, 32, CHINESE_WIND, 1);  // x=42, 16px
    SH1106_ShowColon4(58, 32, 1);  // x=58, 4px精简冒号
    if(fan_fix_speed <= 999)
    {
        SH1106_ShowNum16(62, 32, fan_fix_speed, 1);  // x=62
        SH1106_ShowPercent8(78, 32, 1);   // x=78, % (8px，结束于x=86)
        SH1106_ShowSpace4(86, 32, 1);     // x=86, 4px空格（结束于x=90）
    }
    else
    {
        SH1106_ShowString(62, 32, (uint8_t *)"--", 16, 1);
    }
    
    // 版本号显示 (x=90) - 与第一行"降"位置对齐
    if(device_version != 0)
    {
        // 显示版本号，格式：XX.XX
        uint8_t major_version = (device_version >> 8) & 0xFF;  // 主版本号
        uint8_t minor_version = device_version & 0xFF;         // 子版本号
        
            SH1106_ShowNum16(90, 32, major_version, 1);           // 主版本号 (16px for "10") - x=90, 结束后x=106
            SH1106_ShowDot4(106, 32, 1);                          // 点号 (4px) - x=106-110
            SH1106_ShowNum16(110, 32, minor_version, 1);          // 子版本号 (16px for "69") - x=110
    }
    
    // === 第4行 (y=48) ===
    // 水:36 或 水-36 (x=0) - 使用4像素精简字符
    SH1106_ShowChinese(0, 48, CHINESE_WATER, 1);
    temp_int = water_temp / 10;
    if(temp_int >= -999 && temp_int <= 999)
    {
        if(temp_int >= 0)
        {
            SH1106_ShowColon4(16, 48, 1);  // 正数显示4px冒号
            SH1106_ShowNum16(20, 48, temp_int, 1);  // 数字从x=20开始
        }
        else
        {
            SH1106_ShowMinus4(16, 48, 1);  // 负数显示4px负号
            SH1106_ShowNum16(20, 48, -temp_int, 1);  // 显示绝对值
        }
    }
    else
    {
        SH1106_ShowColon4(16, 48, 1);
        SH1106_ShowString(20, 48, (uint8_t *)"--", 16, 1);
    }
    
    // 连接状态显示 (x=42-88) - 与"功:xx"对齐
    SH1106_ShowSpace8(36, 48, 1);  // 8px空格
    if(conn_status == 0)
    {
        // 未连接状态 - 显示"未连接"
        SH1106_ShowChinese(42, 48, CHINESE_NOT, 1);         // "未" (16px)
        SH1106_ShowChinese(58, 48, CHINESE_CONNECT, 1);     // "连" (16px)
        SH1106_ShowChinese(74, 48, CHINESE_CONNECT_2, 1);   // "接" (16px)
    }
    else if(conn_status >= 1 && conn_status <= 7)
    {
        // 连接过程中显示"连:1/7"到"连:7/7"
        SH1106_ShowChinese(42, 48, CHINESE_CONNECT, 1);     // "连" (16px) - x=42-58
        SH1106_ShowColon4(58, 48, 1);                       // ":" (4px) - x=58-62
        SH1106_ShowNum16(62, 48, conn_status, 1);           // 当前阶段 (8px) - x=62-70
        SH1106_ShowString(70, 48, (uint8_t *)"/7", 16, 1);  // "/7" (16px) - x=70-86
    }
    else if(conn_status == 8)
    {
        // 已连接状态 - 显示"已连接"
        SH1106_ShowChinese(42, 48, CHINESE_ALREADY, 1);     // "已" (16px)
        SH1106_ShowChinese(58, 48, CHINESE_CONNECT, 1);     // "连" (16px)
        SH1106_ShowChinese(74, 48, CHINESE_CONNECT_2, 1);   // "接" (16px)
    }
    else
    {
        // 未知状态
        SH1106_ShowString(42, 48, (uint8_t *)"---", 16, 1);
    }
    
    // 模:CC (x=90) - 使用4像素精简冒号
    // 根据mode_type选择显示的汉字
    switch(mode_type)
    {
        case 1:  mode_char_index = CHINESE_MODE_WEN; break;
        case 2:  mode_char_index = CHINESE_MODE_YU; break;
        case 3:  mode_char_index = CHINESE_MODE_ZHUAN; break;
        case 4:  mode_char_index = CHINESE_MODE_LIAN; break;
        case 7:  mode_char_index = CHINESE_MODE_LENG; break;
        case 8:  mode_char_index = CHINESE_MODE_GUAN; break;
        case 10: mode_char_index = CHINESE_MODE_WEI; break;
        case 11: mode_char_index = CHINESE_MODE_CUO; break;
        case 0xFF: mode_char_index = CHINESE_MODE_DUAN; break;  // 未连接显示"断"
        default: mode_char_index = CHINESE_MODE_CUO; break;
    }
    SH1106_ShowChinese(90, 48, CHINESE_MODE, 1);     // x=90, "模" (16px)
    SH1106_ShowColon4(106, 48, 1);                    // x=106, ":" (4px精简冒号)
    SH1106_ShowChinese(110, 48, mode_char_index, 1);  // x=110, 模式汉字 (16px)
    
    // 刷新显示
    SH1106_Refresh();
#endif
}

#endif  // ENABLE_OLED_DISPLAY
