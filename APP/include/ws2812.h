/*
 * ws2812.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Lee
 */

#ifndef INCLUDE_WS2812_H_
#define INCLUDE_WS2812_H_

#include "CH58x_common.h"

// ������ɫ
#define RED_COLOR   0xFF0000  // ��ɫ
#define GREEN_COLOR 0x00FF00  // ��ɫ


#define RED     0xFF0000  // ��ɫ
#define GREEN   0x00FF00  // ��ɫ
#define BLUE    0x0000FF  // ��ɫ
#define WHITE   0xFFFFFF  // ��ɫ
#define BLACK   0x000000  // �ر�
#define Purple   0x800080  // ��ɫ
#define LightPink  0xFFB6C1  // ǳ�ۺ� http://pauli.cn/tool/color.htm

#define DarkBlue   0x00008B  // ����ɫ


#define INI_STATE  GREEN  


void ws2812_ini(void);
void setDimColor(uint32_t color, float brightness);
#endif /* INCLUDE_WS2812_H_ */
