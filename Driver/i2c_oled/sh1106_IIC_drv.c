/********************************** (C) COPYRIGHT *******************************
 * File Name          : sh1106_IIC_drv.c
 * Author             : Raditor
 * Version            : V1.0
 * Date               : 2024/10/08
 * Description        : SH1106 OLED driver - 使用硬件IIC接口
 *******************************************************************************/

#include "sh1106_IIC_drv.h"
#include "app_i2c.h"
#include "CH58x_common.h"

// 字体数据
#include "oledfont.h"

// OLED显存 - 初始化为0
uint8_t OLED_GRAM_1106[SH1106_WIDTH][8] = {0};

/**
 * @brief 向SH1106写一个字节
 * @param dat 要写入的数据
 * @param mode 0:命令 1:数据
 */
static void SH1106_WR_Byte(uint8_t dat, uint8_t mode)
{
    uint8_t data[2];
    data[0] = mode ? 0x40 : 0x00;  // 0x00:命令 0x40:数据
    data[1] = dat;
    
    i2c_write_to(SH1106_ADDR, data, 2, true, true);
}

/**
 * @brief 打开OLED显示
 */
void SH1106_Display_On(void)
{
    SH1106_WR_Byte(0x8D, SH1106_CMD);
    SH1106_WR_Byte(0x14, SH1106_CMD);
    SH1106_WR_Byte(0xAF, SH1106_CMD);
}

/**
 * @brief 关闭OLED显示
 */
void SH1106_Display_Off(void)
{
    SH1106_WR_Byte(0x8D, SH1106_CMD);
    SH1106_WR_Byte(0x10, SH1106_CMD);
    SH1106_WR_Byte(0xAE, SH1106_CMD);
}

/**
 * @brief 刷新显存到OLED - 逐字节发送
 */
void SH1106_Refresh(void)
{
    uint8_t i, n;
    
    for(i = 0; i < 8; i++)
    {
        // 设置页地址
        SH1106_WR_Byte(0xB0 + i, SH1106_CMD);
        // 设置列地址低4位
        SH1106_WR_Byte(0x02, SH1106_CMD);
        // 设置列地址高4位
        SH1106_WR_Byte(0x10, SH1106_CMD);
        
        // 逐字节发送数据
        for(n = 0; n < SH1106_WIDTH; n++)
        {
            SH1106_WR_Byte(OLED_GRAM_1106[n][i], SH1106_DATA);
        }
    }
}

/**
 * @brief 清屏函数
 */
void SH1106_Clear(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++)
    {
        for(n = 0; n < SH1106_WIDTH; n++)
        {
            OLED_GRAM_1106[n][i] = 0;
        }
    }
    SH1106_Refresh();
}

/**
 * @brief 画点
 * @param x x坐标(0-127)
 * @param y y坐标(0-63)
 * @param t 1:填充 0:清空
 */
void SH1106_DrawPoint(uint8_t x, uint8_t y, uint8_t t)
{
    uint8_t i, m, n;
    
    if(x >= SH1106_WIDTH || y >= SH1106_HEIGHT)
        return;
    
    i = y / 8;
    m = y % 8;
    n = 1 << m;
    
    if(t)
    {
        OLED_GRAM_1106[x][i] |= n;
    }
    else
    {
        OLED_GRAM_1106[x][i] &= ~n;
    }
}

/**
 * @brief 显示一个字符
 * @param x 起始列坐标(0-127)
 * @param y 起始行坐标(0-63)
 * @param chr 字符
 * @param size 字体大小 8/16
 * @param mode 0:反色显示 1:正常显示
 */
void SH1106_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size, uint8_t mode)
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
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
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
void SH1106_ShowString(uint8_t x, uint8_t y, uint8_t *chr, uint8_t size, uint8_t mode)
{
    while((*chr >= ' ') && (*chr <= '~'))
    {
        SH1106_ShowChar(x, y, *chr, size, mode);
        if(size == 8)
            x += 6;
        else
            x += size / 2;
        
        if(x >= SH1106_WIDTH)  // 换行
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
void SH1106_ShowNum(uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t size, uint8_t mode)
{
    uint8_t t, temp, m = 0;
    uint32_t abs_num;
    
    if(size == 8)
        m = 2;
    
    // 处理负数
    if(num < 0)
    {
        SH1106_ShowChar(x, y, '-', size, mode);
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
        SH1106_ShowChar(x + (size / 2 + m) * t, y, temp + '0', size, mode);
    }
}

/**
 * @brief 显示中文汉字（16x16）
 * @param x 起始列坐标
 * @param y 起始行坐标
 * @param index 汉字索引（见oledfont.h中的定义）
 * @param mode 0:反色显示 1:正常显示
 */
void SH1106_ShowChinese(uint8_t x, uint8_t y, uint8_t index, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    // 添加边界检查，防止数组越界
    if(index >= 25)  // 数组有25个元素（索引0-24）
    {
        return;  // 索引超出范围，直接返回
    }
    
    for(i = 0; i < 32; i++)
    {
        temp = chinese_temp_16x16[index][i];
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 16)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示16x16冒号
 */
void SH1106_ShowColon16(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 16; i++)
    {
        temp = num_16x16[10][i];  // 冒号是索引10
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 8)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示8x16冒号（紧凑版）
 */
void SH1106_ShowColon8(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 16; i++)
    {
        temp = char_8x16[0][i];  // 冒号
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 8)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示8x16空格
 */
void SH1106_ShowSpace8(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 16; i++)
    {
        temp = char_8x16[1][i];  // 空格
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 8)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示4x16精简空格
 */
void SH1106_ShowSpace4(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 8; i++)  // 4x16只需要8字节
    {
        temp = char_4x16[1][i];  // 空格（索引1）
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 4)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示8x16百分号
 */
void SH1106_ShowPercent8(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 16; i++)
    {
        temp = char_8x16[2][i];  // 百分号是索引2
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 8)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示16x16数字（不显示前导0）
 * @param x 起始x坐标
 * @param y 起始y坐标  
 * @param num 要显示的数字（支持负数）
 * @param mode 0:反色 1:正常
 */
void SH1106_ShowNum16(uint8_t x, uint8_t y, int32_t num, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0, y0;
    uint8_t temp;
    char str[12];
    uint8_t len;
    
    // 处理负号
    if(num < 0)
    {
        // 显示16x16负号
        x0 = x;
        y0 = y;
        for(i = 0; i < 16; i++)
        {
            temp = num_16x16[11][i];  // 负号是索引11
            for(m = 0; m < 8; m++)
            {
                if(temp & 0x01)
                    SH1106_DrawPoint(x, y, mode);
                else
                    SH1106_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if((x - x0) == 8)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
        x += 8;  // 移动到数字起始位置
        num = -num;
    }
    
    // 转换为字符串（不含前导0）
    len = 0;
    if(num == 0)
    {
        str[len++] = '0';
    }
    else
    {
        int32_t tmp = num;
        int32_t divisor = 1;
        while(tmp / divisor >= 10) divisor *= 10;
        
        while(divisor > 0)
        {
            str[len++] = '0' + (num / divisor) % 10;
            divisor /= 10;
        }
    }
    
    // 显示每个数字
    uint8_t y_start = y;  // 保存起始y坐标
    for(uint8_t idx = 0; idx < len; idx++)
    {
        uint8_t digit = str[idx] - '0';
        x0 = x;
        y0 = y_start;  // 每个数字都从起始y坐标开始
        y = y_start;
        
        for(i = 0; i < 16; i++)
        {
            temp = num_16x16[digit][i];
            for(m = 0; m < 8; m++)
            {
                if(temp & 0x01)
                    SH1106_DrawPoint(x, y, mode);
                else
                    SH1106_DrawPoint(x, y, !mode);
                temp >>= 1;
                y++;
            }
            x++;
            if((x - x0) == 8)
            {
                x = x0;
                y0 = y0 + 8;
            }
            y = y0;
        }
        x += 8;  // 移动到下一个数字位置
    }
}

/**
 * @brief 显示4x16精简冒号
 */
void SH1106_ShowColon4(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 8; i++)  // 4x16只需要8字节
    {
        temp = char_4x16[0][i];  // 精简冒号（索引0）
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 4)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief 显示4x16精简负号
 */
void SH1106_ShowMinus4(uint8_t x, uint8_t y, uint8_t mode)
{
    uint8_t i, m;
    uint8_t x0 = x, y0 = y;
    uint8_t temp;
    
    for(i = 0; i < 8; i++)  // 4x16只需要8字节
    {
        temp = char_4x16[2][i];  // 精简负号（索引2）
        for(m = 0; m < 8; m++)
        {
            if(temp & 0x01)
                SH1106_DrawPoint(x, y, mode);
            else
                SH1106_DrawPoint(x, y, !mode);
            temp >>= 1;
            y++;
        }
        x++;
        if((x - x0) == 4)
        {
            x = x0;
            y0 = y0 + 8;
        }
        y = y0;
    }
}

/**
 * @brief OLED初始化
 */
void SH1106_Init(void)
{
    // 等待电源稳定
    mDelaymS(100);
    
    // 初始化SH1106
    SH1106_WR_Byte(0xAE, SH1106_CMD); /*display off*/
    SH1106_WR_Byte(0x02, SH1106_CMD); /*set lower column address*/
    SH1106_WR_Byte(0x10, SH1106_CMD); /*set higher column address*/
    SH1106_WR_Byte(0x40, SH1106_CMD); /*set display start line*/
    SH1106_WR_Byte(0xB0, SH1106_CMD); /*set page address*/
    SH1106_WR_Byte(0x81, SH1106_CMD); /*contract control*/
    SH1106_WR_Byte(0xCF, SH1106_CMD); /*128*/
    SH1106_WR_Byte(0xA1, SH1106_CMD); /*set segment remap*/
    SH1106_WR_Byte(0xA6, SH1106_CMD); /*normal / reverse*/
    SH1106_WR_Byte(0xA8, SH1106_CMD); /*multiplex ratio*/
    SH1106_WR_Byte(0x3F, SH1106_CMD); /*duty = 1/64*/
    SH1106_WR_Byte(0xAD, SH1106_CMD); /*set charge pump enable*/
    SH1106_WR_Byte(0x8B, SH1106_CMD); /* 0x8B 内供 VCC */
    SH1106_WR_Byte(0x33, SH1106_CMD); /*0X30---0X33 set VPP 9V */
    SH1106_WR_Byte(0xC8, SH1106_CMD); /*Com scan direction*/
    SH1106_WR_Byte(0xD3, SH1106_CMD); /*set display offset*/
    SH1106_WR_Byte(0x00, SH1106_CMD); /* 0x20 */
    SH1106_WR_Byte(0xD5, SH1106_CMD); /*set osc division*/
    SH1106_WR_Byte(0x80, SH1106_CMD);
    SH1106_WR_Byte(0xD9, SH1106_CMD); /*set pre-charge period*/
    SH1106_WR_Byte(0x1F, SH1106_CMD); /*0x22*/
    SH1106_WR_Byte(0xDA, SH1106_CMD); /*set COM pins*/
    SH1106_WR_Byte(0x12, SH1106_CMD);
    SH1106_WR_Byte(0xDB, SH1106_CMD); /*set vcomh*/
    SH1106_WR_Byte(0x40, SH1106_CMD);
    
    SH1106_Clear();
    SH1106_WR_Byte(0xAF, SH1106_CMD); /*display ON*/
}
