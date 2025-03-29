/*
 * ws2812.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Lee
 */

#ifndef INCLUDE_WS2812_H_
#define INCLUDE_WS2812_H_

#include "CH58x_common.h"

// 定义颜色
#define RED_COLOR   0xFF0000  // 红色
#define GREEN_COLOR 0x00FF00  // 绿色


#define RED     0xFF0000  // 红色
#define GREEN   0x00FF00  // 绿色
#define BLUE    0x0000FF  // 蓝色
#define WHITE   0xFFFFFF  // 白色
#define BLACK   0x000000  // 关闭
#define Purple   0x800080  // 紫色
#define LightPink  0xFFB6C1  // 浅粉红 http://pauli.cn/tool/color.htm

#define DarkBlue   0x00008B  // 深蓝色


#define INI_STATE  GREEN  


void ws2812_ini(void);
void setDimColor(uint32_t color, float brightness);
#endif /* INCLUDE_WS2812_H_ */
