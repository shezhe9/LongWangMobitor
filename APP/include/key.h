/*
 * key.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Lee
 */

#ifndef INCLUDE_KEY_H_
#define INCLUDE_KEY_H_
#include "CH58x_common.h" // ȷ������оƬͷ�ļ�

// �����¼����Ͷ���
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SINGLE_CLICK = 0x0001,
    KEY_EVENT_DOUBLE_CLICK = 0x0002,
    KEY_EVENT_LONG_PRESS = 0x0004,
    KEY_TEST_SECOND = 0x0008
} KeyEvent;

// ������������¼�
#define KEY_DOUBLE_CLICK_CHECK  0x0010  // ˫������¼�
#define KEY_LONG_PRESSED_CHECK  0x0020  // ��������¼�
#define KEY_NOISE_PRESSED       0x0040  // �����¼�
#define KEY_STATE_UNKOWN                    0x0100  // ����״̬����
#define BUTTON_PRESSED_OVERTIME_ERR         0x0200  // ���³�ʱ���˳���ʱ��û�˴������
#define DOUBULE_PRESSED_OVERTIME_ERR         0x0400  // ˫���ĵڶ��ΰ��³�����˫���ȴ�ʱ��ϵͳû�д�������ӦΪ����һ�ε���

#define SINGLE_PRESSED_RELASE           0x1000  // ����������
#define DOUBULE_PRESSED_RELASE          0x2000  // ˫��������

// ���尴��ȥ��ʱ�䣨���룩
#define KEY_DEBOUNCE_TIME      50   // 50msȥ��ʱ��
#define SINGLE_CLICK_TIME      300  // �������ʱ����
#define DOUBLE_CLICK_TIME      600  // ˫�����ʱ����
#define LONG_PRESS_TIME        1500 // ����ʱ�䣨1.5s��

// ��������
void Key_Init(void);
uint16_t Key_ProcessEvent(uint8_t task_id, uint16_t events);
KeyEvent Key_GetEvent(void);

/*

�����޸İ�������
���������ж�+��ʱ��+��ʱ�����
��ʱ������RTC�Ĵ���������ʱ�������ʱ����ʱ������TMOSϵͳ�� tmos_start_task �������ּ��붨ʱ�¼�
���������������״̬��(����̬BUTTON_IDLE������̬BUTTON_PRESSED���ȴ�˫��̬BUTTON_DOUBLE_CLICK_WAIT)��������ʱ����¼���KEY_DOUBLE_CLICK_CHECK, KEY_LONG_PRESSED_CHECK����
�����߼���
��ʼ����ťΪ�½��ش����ж�
Ĭ�ϰ�ťû�а���ʱ,Ϊ����̬(BUTTON_IDLE)��
���º󴥷������жϣ��޸��ж�Ϊ�����ش�������¼ʱ���1(buttonPressTime),������1.5s�ӳټ���ƽ����(tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, 2400)),Ȼ����밴��̬(BUTTON_PRESSED)��
�Ȱ�ť�ͷź��ж���Ӧ�����أ��жϸ�Ϊ�½��ش�������¼ʱ���2(buttonReleaseTime)������ʱ���2-1����ʱ����Ƿ�С��50ms���ǵĻ���Ϊ��ǰֻ��ë�̣�����(BUTTON_IDLE)��ȡ����������¼�tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK����
���ʱ�������50��С��300ms���ж�Ϊ���ܵ�������˫��������ȴ�˫��̬BUTTON_DOUBLE_CLICK_WAIT,������˫�������¼�KEY_DOUBLE_CLICK_CHECK��
���ʱ�������300ms��С��1.5s���ж�Ϊ������������Ϣ̬

����ť�ٴΰ��£��ж���Ӧ�½��ش�������¼ʱ���3������ʱ���3-2�Ƿ�С��600ms���ǵĻ�˫���¼����������˫���¼���ȡ��˫�������¼������ؿ���̬����Ļ��������ؿ���̬

˫�������¼�����Ϊ����ⰴ����ƽΪ�ߣ�˵���ǵ����������ڵȴ��ˣ���������¼���״̬���Ļؿ���̬������ƽΪ�ͱ�����Ϊ��ť���Ϊ�ͻ����жϽ��д���

���������¼�����Ϊ����ⰴ����ƽΪ�ͣ�˵���ǳ��������õȰ�ť�ͷ��ˣ����ж�Ϊ�½��ش�������������¼�����ť״̬���ظ�����̬�����������ƽΪ�ߣ�˵����ť�ͷ���û��ȡ�����¼���˵���߼������⣬���������ؿ���̬��

�����߼������print��Ҫ��ӡ�ĸ�����������ť�¼����ʱ���øû���
tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK
tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK)
tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS





*/


#endif /* INCLUDE_KEY_H_ */
