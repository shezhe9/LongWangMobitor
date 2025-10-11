/********************************** (C) COPYRIGHT *******************************
 * File Name          : sh1107_i2c_drv.c
 * Author             : Raditor
 * Version            : V1.0
 * Date               : 2024/10/08
 * Description        : SH1107 OLED driver - 使用硬件IIC接口
 *******************************************************************************/

#include "sh1107_i2c_drv.h"
#include "app_i2c.h"
#include "CH58x_common.h"

// 字体数据
#include "oledfont.h"

// OLED显存 - 初始化为0
uint8_t OLED_GRAM[SH1107_WIDTH][16] = {0};

/**
 * @brief 向SH1107写一个字节
 * @param dat 要写入的数据
 * @param mode 0:命令 1:数据
 */
static void SH1107_WR_Byte(uint8_t dat, uint8_t mode)
{
    uint8_t data[2];
    data[0] = mode ? 0x40 : 0x00;  // 0x00:命令 0x40:数据
    data[1] = dat;
    
    i2c_write_to(SH1107_ADDR, data, 2, true, true);
}

/**
 * @brief 打开OLED显示
 */
void SH1107_Display_On(void)
{
    SH1107_WR_Byte(0xAF, SH1107_CMD);
}

/**
 * @brief 关闭OLED显示
 */
void SH1107_Display_Off(void)
{
    SH1107_WR_Byte(0xAE, SH1107_CMD);
}

/**
 * @brief 刷新显存到OLED
 */
void SH1107_Refresh(void)
{
    uint8_t i, n;
    uint8_t data[SH1107_WIDTH + 1];
    
    for(i = 0; i < 16; i++)
    {
        // 设置页地址
        SH1107_WR_Byte(0xB0 + i, SH1107_CMD);
        // 设置列地址低4位
        SH1107_WR_Byte(0x00, SH1107_CMD);
        // 设置列地址高4位
        SH1107_WR_Byte(0x10, SH1107_CMD);
        
        // 准备数据：第一个字节是数据命令标志
        data[0] = 0x40;
        for(n = 0; n < SH1107_WIDTH; n++)
        {
            data[n + 1] = OLED_GRAM[n][i];
        }
        
        // 发送一页的数据
        i2c_write_to(SH1107_ADDR, data, SH1107_WIDTH + 1, true, true);
    }
}

/**
 * @brief 清屏函数
 */
void SH1107_Clear(void)
{
    uint8_t i, n;
    for(i = 0; i < 16; i++)
    {
        for(n = 0; n < SH1107_WIDTH; n++)
        {
            OLED_GRAM[n][i] = 0;
        }
    }
    SH1107_Refresh();
}

/**
 * @brief 画点
 * @param x x坐标(0-63)
 * @param y y坐标(0-127)
 * @param t 1:填充 0:清空
 */
void SH1107_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    
    if(x >= SH1107_WIDTH || y >= SH1107_HEIGHT)
        return;
    
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    
    if(t)
    {
        OLED_GRAM[x][i] |= n;
    }
    else
    {
        OLED_GRAM[x][i] &= ~n;
    }
}

/**
 * @brief 显示一个字符
 * @param x 起始列坐标(0-63)
 * @param y 起始行坐标(0-127)
 * @param chr 字符
 * @param size 字体大小 8/16
 * @param mode 0:反色显示 1:正常显示
 */
void SH1107_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
{
    uint8_t i, m, temp, size2, chr1;
    uint8_t x0 = x, y0 = y;
    
    if(size == 8)
        size2 = 6;
    else
        size2 = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);
    
    chr1 = chr - ' ';
    
    for(i = 0; i < size2; i++)
    {
        if(size == 8)
            temp = asc2_0806[chr1][i];
        else if(size == 16)
            temp = asc2_1608[chr1][i];
        else
            return;
        
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1107_DrawPoint(x, y, mode);
            else
                SH1107_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((size != 8) && ((x - x0) == size / 2))
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示字符串
 * @param x 起始列坐标
 * @param y 起始行坐标
 * @param chr 字符串指针
 * @param size 字体大小 8/16
 * @param mode 0:反色显示 1:正常显示
 */
void SH1107_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size, uint8_t mode)
{
    while((*chr >= ' ') && (*chr <= '~'))
    {
        SH1107_ShowChar(x, y, *chr, size, mode);
        if(size == 8)
            x += 6;
        else
            x += size / 2;
        
        if(x >= SH1107_WIDTH)  // 换行
        {
            x = 0;
            y += size;
        }
        chr++;
    }
}

/**
 * @brief m^n函数
 */
static uint32_t OLED_Pow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--)
    {
        result *= m;
    }
    return result;
}

/**
 * @brief 显示数字
 * @param x 起始列坐标
 * @param y 起始行坐标
 * @param num 要显示的数字（支持负数）
 * @param len 数字的位数
 * @param size 字体大小 8/16
 * @param mode 0:反色显示 1:正常显示
 */
void SH1107_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp, m = 0;
    uint32_t abs_num;
    
    if(size == 8)
        m = 2;
    
    // 处理负数
    if(num < 0)
    {
        SH1107_ShowChar(x, y, '-', size, mode);
        abs_num = -num;
        x += (size / 2 + m);
    }
    else
    {
        abs_num = num;
    }
    
    for(t = 0; t < len; t++)
    {
        temp = (abs_num / OLED_Pow(10, len - t - 1)) % 10;
        SH1107_ShowChar(x + (size / 2 + m) * t, y, temp + '0', size, mode);
    }
}

/**
 * @brief OLED初始化
 */
void SH1107_Init(void)
{
    // 等待电源稳定
    mDelaymS(100);
    
    // 初始化SH1107
    SH1107_WR_Byte(0xAE, SH1107_CMD);  // 关闭显示
    
    SH1107_WR_Byte(0x00, SH1107_CMD);  // 设置列地址低4位
    SH1107_WR_Byte(0x10, SH1107_CMD);  // 设置列地址高4位
    
    SH1107_WR_Byte(0xB0, SH1107_CMD);  // 设置页地址
    
    SH1107_WR_Byte(0xDC, SH1107_CMD);  // 设置显示起始行
    SH1107_WR_Byte(0x00, SH1107_CMD);
    
    SH1107_WR_Byte(0x81, SH1107_CMD);  // 对比度控制
    SH1107_WR_Byte(0x2F, SH1107_CMD);
    
    SH1107_WR_Byte(0x20, SH1107_CMD);  // 设置内存地址模式
    
    SH1107_WR_Byte(0xA0, SH1107_CMD);  // 列地址0映射到SEG0
    
    SH1107_WR_Byte(0xC0, SH1107_CMD);  // 正常扫描方向
    
    SH1107_WR_Byte(0xA8, SH1107_CMD);  // 复用比
    SH1107_WR_Byte(0x3F, SH1107_CMD);  // 1/64 duty
    
    SH1107_WR_Byte(0xD3, SH1107_CMD);  // 设置显示偏移
    SH1107_WR_Byte(0x60, SH1107_CMD);
    
    SH1107_WR_Byte(0xD5, SH1107_CMD);  // 设置振荡器频率
    SH1107_WR_Byte(0x51, SH1107_CMD);
    
    SH1107_WR_Byte(0xD9, SH1107_CMD);  // 设置预充电周期
    SH1107_WR_Byte(0x22, SH1107_CMD);
    
    SH1107_WR_Byte(0xDB, SH1107_CMD);  // 设置VCOMH
    SH1107_WR_Byte(0x35, SH1107_CMD);
    
    SH1107_WR_Byte(0xAD, SH1107_CMD);  // DC-DC使能
    SH1107_WR_Byte(0x8A, SH1107_CMD);
    
    SH1107_WR_Byte(0xA4, SH1107_CMD);  // 全局显示开启
    SH1107_WR_Byte(0xA6, SH1107_CMD);  // 正常显示
    
    SH1107_Clear();
    SH1107_WR_Byte(0xAF, SH1107_CMD);  // 打开显示
}
