/********************************** (C) COPYRIGHT *******************************
 * File Name          : central.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        : 观察应用主函数及任务系统初始化
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

// 新增：蓝牙发送测试数据事件
#define START_SEND_TEST_DATA_EVT      0x0400
// 新增：连接成功后发送初始化数据事件
#define START_SEND_INIT_DATA_EVT      0x0800

// 新增：连接控制事件
#define STOP_AUTO_RECONNECT_EVT       0x1000  // 停止自动重连事件
#define START_AUTO_RECONNECT_EVT      0x2000  // 启动自动重连事件

// 新增：目标设备服务和特征UUID定义
#define TARGET_SERVICE_UUID           0xAE00  // 目标服务UUID
#define TARGET_WRITE_CHAR_UUID        0xAE10  // 写特征UUID (用于发送数据)
#define TARGET_NOTIFY_CHAR_UUID       0xAE02  // 通知特征UUID (用于接收数据)

// 新增：目标设备名称
#define TARGET_DEVICE_NAME            "HID-LongWang"
#define TARGET_DEVICE_NAME_LEN        (sizeof(TARGET_DEVICE_NAME) - 1)  // 减去null终止符

// 新增：测试数据相关常量
#define TEST_DATA_COUNT               20     // 发送的测试数据数量
#define TEST_DATA_VALUE               0xAA   // 测试数据值

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
extern uint8_t centralTaskId;  // Central任务ID，供外部模块使用

/*
 * 新增：连接控制函数声明
 */
extern void Central_DisconnectAndStopAutoReconnect(void);  // 断开连接并停止自动重连
extern void Central_StartAutoReconnect(void);              // 开始自动搜索和连接
extern uint8_t Central_IsConnected(void);                  // 检查是否已连接

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
