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

extern  uint8_t centralTaskId;                                        // 中央设备任务ID

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
#define START_HID_TEST_DATA_EVT       0x0400
#define START_READ_DEVICE_INFO_EVT    0x0800

// HID device matching information
#define TARGET_DEVICE_NAME            "HID-LongWang"
#define TARGET_VENDOR_ID              0x08D8
#define TARGET_PRODUCT_ID             0x0001

// AE00 Service UUIDs (BLE channel for data transmission)
#define AE00_SERVICE_UUID             0xAE00    // AE00 Service
#define AE10_CHAR_UUID                0xAE10    // AE10 Characteristic

// Device Information Service UUIDs
#define DEVICE_INFO_SERVICE_UUID      0x180A
#define VENDOR_ID_CHAR_UUID           0x2A23
#define PRODUCT_ID_CHAR_UUID          0x2A24

// Extended device information structure
typedef struct
{
    uint8_t  addr[B_ADDR_LEN];        // Device address
    uint8_t  addrType;                // Address type
    uint8_t  deviceName[32];          // Device name
    uint8_t  nameLen;                 // Device name length
    uint16_t vendorId;                // Vendor ID
    uint16_t productId;               // Product ID
    uint8_t  isTargetDevice;          // Flag indicating if this is our target device
} extDeviceInfo_t;

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
 * Send BLE test data to connected device
 */
extern BOOL SendHIDTestData(void);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
