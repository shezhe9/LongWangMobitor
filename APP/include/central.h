/********************************** (C) COPYRIGHT *******************************
 * File Name          : central.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        : �۲�Ӧ��������������ϵͳ��ʼ��
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef CENTRAL_H
#define CENTRAL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Simple BLE Observer Task Events
#define START_DEVICE_EVT              0x0001
#define START_DISCOVERY_EVT           0x0002
#define START_SCAN_EVT                0x0004
#define START_SVC_DISCOVERY_EVT       0x0008
#define START_PARAM_UPDATE_EVT        0x0010
#define START_PHY_UPDATE_EVT          0x0020
#define START_READ_OR_WRITE_EVT       0x0040
#define START_WRITE_CCCD_EVT          0x0080
#define START_READ_RSSI_EVT           0x0100
#define ESTABLISH_LINK_TIMEOUT_EVT    0x0200

// �������������Ͳ��������¼�
#define START_SEND_TEST_DATA_EVT      0x0400
// ���������ӳɹ����ͳ�ʼ�������¼�
#define START_SEND_INIT_DATA_EVT      0x0800

// ���������ӿ����¼�
#define STOP_AUTO_RECONNECT_EVT       0x1000  // ֹͣ�Զ������¼�
#define START_AUTO_RECONNECT_EVT      0x2000  // �����Զ������¼�

// ������Ŀ���豸���������UUID����
#define TARGET_SERVICE_UUID           0xAE00  // Ŀ�����UUID
#define TARGET_WRITE_CHAR_UUID        0xAE10  // д����UUID (���ڷ�������)
#define TARGET_NOTIFY_CHAR_UUID       0xAE02  // ֪ͨ����UUID (���ڽ�������)

// ������Ŀ���豸����
#define TARGET_DEVICE_NAME            "HID-LongWang"
#define TARGET_DEVICE_NAME_LEN        (sizeof(TARGET_DEVICE_NAME) - 1)  // ��ȥnull��ֹ��

// ����������������س���
#define TEST_DATA_COUNT               20     // ���͵Ĳ�����������
#define TEST_DATA_VALUE               0xAA   // ��������ֵ

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the BLE Application
 */
extern void Central_Init(void);

/*
 * Task Event Processor for the BLE Application
 */
extern uint16_t Central_ProcessEvent(uint8_t task_id, uint16_t events);

/*
 * External variables for accessing central task ID
 */
extern uint8_t centralTaskId;  // Central����ID�����ⲿģ��ʹ��

/*
 * ���������ӿ��ƺ�������
 */
extern void Central_DisconnectAndStopAutoReconnect(void);  // �Ͽ����Ӳ�ֹͣ�Զ�����
extern void Central_StartAutoReconnect(void);              // ��ʼ�Զ�����������
extern uint8_t Central_IsConnected(void);                  // ����Ƿ�������

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
