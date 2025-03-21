/********************************** (C) COPYRIGHT *******************************
* File Name          : central.c                                                // 文件名：central.c
* Author             : WCH                                                      // 作者：WCH
* Version            : V1.1                                                     // 版本：V1.1
* Date               : 2020/08/06                                              // 日期：2020/08/06
* Description        : 主机例程，主动扫描周围设备，连接至给定的从机设备地址，           // 描述：主机示例程序
*                      寻找自定义服务及特征，执行读写命令，需与从机例程配合使用,        //      扫描并连接指定从机
                       并将从机设备地址修改为该例程目标地址，默认为(84:C2:E4:03:02:02)  //      默认目标地址
*******************************************************************************/

/*********************************************************************
 * INCLUDES                                                           // 包含头文件
 */
#include "CONFIG.h"                                                   // 包含配置头文件
#include "gattprofile.h"                                             // 包含GATT配置文件头文件
#include "central.h"                                                 // 包含central主机头文件
#include "key.h"   
/*********************************************************************
 * MACROS                                                            // 宏定义
 */

// Length of bd addr as a string                                     
#define B_ADDR_STR_LEN                      15                       // 蓝牙地址字符串长度定义为15

/*********************************************************************
 * CONSTANTS                                                         // 常量定义
 */
// Maximum number of scan responses                                  
#define DEFAULT_MAX_SCAN_RES                50                       // 最大扫描响应数量为50

// Scan duration in 0.625ms                                         
#define DEFAULT_SCAN_DURATION               2400                     // 扫描持续时间，单位0.625ms

// Connection min interval in 1.25ms                                
#define DEFAULT_MIN_CONNECTION_INTERVAL     20                       // 最小连接间隔，单位1.25ms

// Connection max interval in 1.25ms                                
#define DEFAULT_MAX_CONNECTION_INTERVAL     100                      // 最大连接间隔，单位1.25ms

// Connection supervision timeout in 10ms                           
#define DEFAULT_CONNECTION_TIMEOUT          100                      // 连接监督超时，单位10ms

// Discovey mode (limited, general, all)                            
#define DEFAULT_DISCOVERY_MODE              DEVDISC_MODE_ALL         // 发现模式设置为全部发现

// TRUE to use active scan                                          
#define DEFAULT_DISCOVERY_ACTIVE_SCAN       TRUE                     // 使用主动扫描

// TRUE to use white list during discovery                          
#define DEFAULT_DISCOVERY_WHITE_LIST        FALSE                    // 不使用白名单进行发现

// TRUE to use high scan duty cycle when creating link              
#define DEFAULT_LINK_HIGH_DUTY_CYCLE        FALSE                   // 创建连接时不使用高占空比扫描

// TRUE to use white list when creating link                        
#define DEFAULT_LINK_WHITE_LIST             FALSE                   // 创建连接时不使用白名单

// Default read RSSI period in 0.625ms                             
#define DEFAULT_RSSI_PERIOD                 2400                    // 默认RSSI读取周期，单位0.625ms

// Minimum connection interval (units of 1.25ms)                    
#define DEFAULT_UPDATE_MIN_CONN_INTERVAL    20                      // 更新连接参数的最小间隔

// Maximum connection interval (units of 1.25ms)                    
#define DEFAULT_UPDATE_MAX_CONN_INTERVAL    100                     // 更新连接参数的最大间隔

// Slave latency to use parameter update                            
#define DEFAULT_UPDATE_SLAVE_LATENCY        0                       // 从机延迟参数

// Supervision timeout value (units of 10ms)                        
#define DEFAULT_UPDATE_CONN_TIMEOUT         600                     // 连接超时值，单位10ms

// Default passcode                                                 
#define DEFAULT_PASSCODE                    0                       // 默认配对密码

// Default GAP pairing mode                                         
#define DEFAULT_PAIRING_MODE                GAPBOND_PAIRING_MODE_INITIATE // 默认GAP配对模式为发起配对

// Default MITM mode (TRUE to require passcode or OOB when pairing) 
#define DEFAULT_MITM_MODE                   TRUE                    // 默认启用MITM保护

// Default bonding mode, TRUE to bond, max bonding 6 devices        
#define DEFAULT_BONDING_MODE                TRUE                    // 默认启用绑定模式，最多绑定6个设备

// Default GAP bonding I/O capabilities                             
#define DEFAULT_IO_CAPABILITIES             GAPBOND_IO_CAP_DISPLAY_ONLY // 默认GAP绑定I/O能力为仅显示

// Default service discovery timer delay in 0.625ms                 
#define DEFAULT_SVC_DISCOVERY_DELAY         1600                    // 默认服务发现延时

// Default parameter update delay in 0.625ms                        
#define DEFAULT_PARAM_UPDATE_DELAY          3200                    // 默认参数更新延时

// Default phy update delay in 0.625ms                              
#define DEFAULT_PHY_UPDATE_DELAY            2400                    // 默认PHY更新延时

// Default read or write timer delay in 0.625ms                     
#define DEFAULT_READ_OR_WRITE_DELAY         1600                    // 默认读写操作延时

// Default write CCCD delay in 0.625ms                              
#define DEFAULT_WRITE_CCCD_DELAY            1600                    // 默认写CCCD延时

// Establish link timeout in 0.625ms                                
#define ESTABLISH_LINK_TIMEOUT              3200                    // 建立连接超时时间

// Application states                                               // 应用状态枚举
enum
{
    BLE_STATE_IDLE,                                                // 空闲状态
    BLE_STATE_CONNECTING,                                          // 正在连接状态
    BLE_STATE_CONNECTED,                                          // 已连接状态
    BLE_STATE_DISCONNECTING                                       // 正在断开连接状态
};

// Discovery states                                                // 发现状态枚举
enum
{
    BLE_DISC_STATE_IDLE,                                         // 空闲状态
    BLE_DISC_STATE_SVC,                                          // 服务发现状态
    BLE_DISC_STATE_CHAR,                                         // 特征发现状态
    BLE_DISC_STATE_CCCD                                          // CCCD发现状态
};

/*********************************************************************
 * TYPEDEFS                                                          // 类型定义
 */

/*********************************************************************
 * GLOBAL VARIABLES                                                  // 全局变量
 */

/*********************************************************************
 * EXTERNAL VARIABLES                                                // 外部变量
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS                                                // 外部函数
 */

/*********************************************************************
 * LOCAL VARIABLES                                                   // 本地变量
 */

// Task ID for internal task/event processing                        
static uint8_t centralTaskId;                                        // 中央设备任务ID

// Number of scan results                                            
static uint8_t centralScanRes;                                       // 扫描结果数量

// Scan result list                                                  
static gapDevRec_t centralDevList[DEFAULT_MAX_SCAN_RES];            // 扫描结果列表

// Peer device address                                               
static uint8_t PeerAddrDef[B_ADDR_LEN] = {0x03, 0x03, 0x03, 0xE4, 0xC2, 0x84}; // 对端设备地址

// RSSI polling state                                                
static uint8_t centralRssi = TRUE;                                   // RSSI轮询状态

// Parameter update state                                            
static uint8_t centralParamUpdate = TRUE;                           // 参数更新状态

// Phy update state                                                  
static uint8 centralPhyUpdate = FALSE;                              // PHY更新状态

// Connection handle of current connection                           
static uint16_t centralConnHandle = GAP_CONNHANDLE_INIT;            // 当前连接句柄

// Application state                                                 
static uint8_t centralState = BLE_STATE_IDLE;                       // 应用状态

// Discovery state                                                   
static uint8_t centralDiscState = BLE_DISC_STATE_IDLE;              // 发现状态

// Discovered service start and end handle                           
static uint16_t centralSvcStartHdl = 0;                             // 发现的服务起始句柄
static uint16_t centralSvcEndHdl = 0;                               // 发现的服务结束句柄

// Discovered characteristic handle                                   
static uint16_t centralCharHdl = 0;                                 // 发现的特征句柄

// Discovered Client Characteristic Configuration handle              
static uint16_t centralCCCDHdl = 0;                                 // 发现的客户端特征配置句柄

// Value to write                                                    
static uint8_t centralCharVal = 0x5A;                               // 要写入的值

// Value read/write toggle                                           
static uint8_t centralDoWrite = TRUE;                               // 读/写切换标志

// GATT read/write procedure state                                   
static uint8_t centralProcedureInProgress = FALSE;                  // GATT读/写过程状态

/*********************************************************************
 * LOCAL FUNCTIONS                                                   // 本地函数声明
 */
static void centralProcessGATTMsg(gattMsgEvent_t *pMsg);            // 处理GATT消息
static void centralRssiCB(uint16_t connHandle, int8_t rssi);        // RSSI回调函数
static void centralEventCB(gapRoleEvent_t *pEvent);                 // 事件回调函数
static void centralHciMTUChangeCB(uint16_t connHandle, uint16_t maxTxOctets, uint16_t maxRxOctets); // MTU变更回调
static void centralPasscodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                              uint8_t uiInputs, uint8_t uiOutputs);  // 密码回调函数
static void centralPairStateCB(uint16_t connHandle, uint8_t state, uint8_t status); // 配对状态回调
static void central_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);         // 处理TMOS消息
static void centralGATTDiscoveryEvent(gattMsgEvent_t *pMsg);        // GATT发现事件处理
static void centralStartDiscovery(void);                            // 开始服务发现
static void centralAddDeviceInfo(uint8_t *pAddr, uint8_t addrType); // 添加设备信息

/*********************************************************************
 * PROFILE CALLBACKS                                                 // 配置文件回调
 */

// GAP Role Callbacks                                                // GAP角色回调结构体
static gapCentralRoleCB_t centralRoleCB = {
    centralRssiCB,        // RSSI callback                          // RSSI回调
    centralEventCB,       // Event callback                         // 事件回调
    centralHciMTUChangeCB // MTU change callback                    // MTU变更回调
};

// Bond Manager Callbacks                                            // 绑定管理器回调结构体
static gapBondCBs_t centralBondCB = {
    centralPasscodeCB,    // Passcode callback                      // 密码回调
    centralPairStateCB    // Pairing state callback                 // 配对状态回调
};

/*********************************************************************
 * PUBLIC FUNCTIONS                                                  // 公共函数
 */

/*********************************************************************
 * @fn      Central_Init
 *
 * @brief   Initialization function for the Central App Task.        // 中央设备应用任务的初始化函数
 *          This is called during initialization and should contain  // 这个函数在初始化时被调用，应该包含
 *          any application specific initialization (ie. hardware    // 任何应用程序特定的初始化（如硬件
 *          initialization/setup, table initialization, power up     // 初始化/设置，表初始化，上电
 *          notification).                                          // 通知）
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be   // task_id - TMOS分配的ID，这个ID应该
 *                    used to send messages and set timers.         // 用于发送消息和设置定时器
 *
 * @return  none                                                    // 无返回值
 */
void Central_Init()
{
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);  // 注册中央设备事件处理函数
    
    // Setup GAP                                                      // 设置GAP参数
    GAP_SetParamValue(TGAP_DISC_SCAN, DEFAULT_SCAN_DURATION);        // 设置扫描持续时间
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, DEFAULT_MIN_CONNECTION_INTERVAL); // 设置最小连接间隔
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, DEFAULT_MAX_CONNECTION_INTERVAL); // 设置最大连接间隔
    GAP_SetParamValue(TGAP_CONN_EST_SUPERV_TIMEOUT, DEFAULT_CONNECTION_TIMEOUT); // 设置连接超时时间

    // Setup the GAP Bond Manager                                     // 设置GAP绑定管理器
    {
        uint32_t passkey = DEFAULT_PASSCODE;                          // 设置默认密码
        uint8_t  pairMode = DEFAULT_PAIRING_MODE;                    // 设置配对模式
        uint8_t  mitm = DEFAULT_MITM_MODE;                           // 设置MITM保护模式
        uint8_t  ioCap = DEFAULT_IO_CAPABILITIES;                    // 设置IO能力
        uint8_t  bonding = DEFAULT_BONDING_MODE;                     // 设置绑定模式

        GAPBondMgr_SetParameter(GAPBOND_CENT_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);  // 设置默认密码
        GAPBondMgr_SetParameter(GAPBOND_CENT_PAIRING_MODE, sizeof(uint8_t), &pairMode);      // 设置配对模式
        GAPBondMgr_SetParameter(GAPBOND_CENT_MITM_PROTECTION, sizeof(uint8_t), &mitm);       // 设置MITM保护
        GAPBondMgr_SetParameter(GAPBOND_CENT_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);      // 设置IO能力
        GAPBondMgr_SetParameter(GAPBOND_CENT_BONDING_ENABLED, sizeof(uint8_t), &bonding);    // 设置绑定使能
    }

    // Initialize GATT Client                                         // 初始化GATT客户端
    GATT_InitClient();
    // Register to receive incoming ATT Indications/Notifications     // 注册接收ATT指示/通知
    GATT_RegisterForInd(centralTaskId);
    // Setup a delayed profile startup                               // 设置延迟的配置文件启动
    tmos_set_event(centralTaskId, START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      Central_ProcessEvent
 *
 * @brief   Central Application Task event processor.                // 中央应用任务事件处理器
 *          This function is called to process all events for the task. // 这个函数被调用来处理任务的所有事件
 *          Events include timers, messages and any other user defined  // 事件包括定时器、消息和任何其他用户定义的事件
 *          events.
 *
 * @param   task_id  - The TMOS assigned task ID.                   // task_id - TMOS分配的任务ID
 * @param   events - events to process.  This is a bit map and can  // events - 要处理的事件，这是一个位图
 *                   contain more than one event.                   // 可以包含多个事件
 *
 * @return  events not processed                                    // 返回未处理的事件
 */
uint16_t Central_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)                                       // 如果是系统消息事件
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(centralTaskId)) != NULL)         // 接收消息
        {
            central_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);        // 处理TMOS消息
            // Release the TMOS message                               // 释放TMOS消息
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events                                 // 返回未处理的事件
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & START_DEVICE_EVT)                                    // 如果是启动设备事件
    {
        // Start the Device                                          // 启动设备
        GAPRole_CentralStartDevice(centralTaskId, &centralBondCB, &centralRoleCB);
        return (events ^ START_DEVICE_EVT);
    }

    if(events & ESTABLISH_LINK_TIMEOUT_EVT)                              // 如果是建立连接超时事件
    {
        GAPRole_TerminateLink(INVALID_CONNHANDLE);                    // 终止连接
        return (events ^ ESTABLISH_LINK_TIMEOUT_EVT);
    }

    if(events & START_SVC_DISCOVERY_EVT)                              // 如果是开始服务发现事件
    {
        // start service discovery                                     // 开始服务发现
        centralStartDiscovery();
        return (events ^ START_SVC_DISCOVERY_EVT);
    }

    if(events & START_PARAM_UPDATE_EVT)                               // 如果是开始参数更新事件
    {
        // start connect parameter update                              // 开始连接参数更新
        GAPRole_UpdateLink(centralConnHandle,
                           DEFAULT_UPDATE_MIN_CONN_INTERVAL,           // 使用默认的最小连接间隔
                           DEFAULT_UPDATE_MAX_CONN_INTERVAL,           // 使用默认的最大连接间隔
                           DEFAULT_UPDATE_SLAVE_LATENCY,               // 使用默认的从机延迟
                           DEFAULT_UPDATE_CONN_TIMEOUT);               // 使用默认的连接超时
        return (events ^ START_PARAM_UPDATE_EVT);
    }

    if(events & START_PHY_UPDATE_EVT)                                 // 如果是开始PHY更新事件
    {
        // start phy update                                           // 开始PHY更新
        PRINT("PHY Update %x...\n", GAPRole_UpdatePHY(centralConnHandle, 0, 
                    GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, GAP_PHY_OPTIONS_NOPRE));

        return (events ^ START_PHY_UPDATE_EVT);
    }

    if(events & START_READ_OR_WRITE_EVT)                              // 如果是开始读写事件
    {
        if(centralProcedureInProgress == FALSE)                       // 如果当前没有正在进行的操作
        {
            if(centralDoWrite)                                        // 如果是写操作
            {
                // Do a write                                         // 执行写操作
                attWriteReq_t req;

                req.cmd = FALSE;                                      // 不是命令
                req.sig = FALSE;                                      // 不带签名
                req.handle = centralCharHdl;                          // 设置特征句柄

                req.len = 1;                                          // 写入长度为1
                req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // 分配内存
                printf("READ_OR_WRITE_handle = %x\n" ,req.handle);    // 打印句柄值
                if(req.pValue != NULL)                                // 如果内存分配成功
                {
                    *req.pValue = centralCharVal;                     // 设置写入值

                    if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // 写入特征值
                    {
                        centralProcedureInProgress = TRUE;            // 设置操作进行中标志
                        centralDoWrite = !centralDoWrite;             // 切换读写标志
                        tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY); // 启动下一次读写
                    }
                    else
                    {
                        GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ); // 释放内存
                    }
                }
            }
            else
            {
                // Do a read                                          // 执行读操作
                attReadReq_t req;

                req.handle = centralCharHdl;                          // 设置特征句柄
                if(GATT_ReadCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // 读取特征值
                {
                    centralProcedureInProgress = TRUE;                // 设置操作进行中标志
                    centralDoWrite = !centralDoWrite;                 // 切换读写标志
                }
            }
        }
        return (events ^ START_READ_OR_WRITE_EVT);
    }

    if(events & START_WRITE_CCCD_EVT)                                   // 如果是开始写CCCD事件
    {
        if(centralProcedureInProgress == FALSE)                        // 如果当前没有正在进行的操作
        {
            // Do a write                                              // 执行写操作
            attWriteReq_t req;

            req.cmd = FALSE;                                           // 不是命令
            req.sig = FALSE;                                           // 不带签名
            req.handle = centralCCCDHdl;                               // 设置CCCD句柄
            req.len = 2;                                               // 写入长度为2
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // 分配内存
            printf("CCCD_handle = %x\n" ,req.handle);                  // 打印句柄值
            if(req.pValue != NULL)                                     // 如果内存分配成功
            {
                req.pValue[0] = 1;                                     // 启用通知
                req.pValue[1] = 0;

                if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // 写入CCCD值
                {
                    centralProcedureInProgress = TRUE;                 // 设置操作进行中标志
                }
                else
                {
                    GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);   // 释放内存
                }
            }
        }
        return (events ^ START_WRITE_CCCD_EVT);
    }

    if(events & START_READ_RSSI_EVT)                                  // 如果是开始读取RSSI事件
    {
        GAPRole_ReadRssiCmd(centralConnHandle);                       // 读取RSSI值
        tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD); // 启动下一次RSSI读取
        return (events ^ START_READ_RSSI_EVT);
    }

    // Discard unknown events                                         // 丢弃未知事件
    return 0;
}

/*********************************************************************
 * @fn      central_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.                         // 处理传入的任务消息
 *
 * @param   pMsg - message to process                                // pMsg - 要处理的消息
 *
 * @return  none                                                     // 无返回值
 */
static void central_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch(pMsg->event)                                               // 根据消息事件类型处理
    {
        case GATT_MSG_EVENT:                                          // 如果是GATT消息事件
            centralProcessGATTMsg((gattMsgEvent_t *)pMsg);            // 处理GATT消息
            break;
    }
}

/*********************************************************************
 * @fn      centralProcessGATTMsg
 *
 * @brief   Process GATT messages                                    // 处理GATT消息
 *
 * @return  none                                                     // 无返回值
 */
static void centralProcessGATTMsg(gattMsgEvent_t *pMsg)
{
    uint16 i = 0;                                                     // 循环计数器
    if(centralState != BLE_STATE_CONNECTED)                           // 如果不是已连接状态
    {
        // In case a GATT message came after a connection has dropped,  // 如果连接已断开后收到GATT消息
        // ignore the message                                           // 忽略该消息
        GATT_bm_free(&pMsg->msg, pMsg->method);
        return;
    }

    if((pMsg->method == ATT_EXCHANGE_MTU_RSP) ||                        // 如果是MTU交换响应
       ((pMsg->method == ATT_ERROR_RSP) &&                            // 或者是错误响应
        (pMsg->msg.errorRsp.reqOpcode == ATT_EXCHANGE_MTU_REQ)))      // 且是MTU交换请求的错误
    {
        if(pMsg->method == ATT_ERROR_RSP)                             // 如果是错误响应
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;              // 获取错误码

            PRINT("Exchange MTU Error: %x\n", status);                 // 打印MTU交换错误
        }
        centralProcedureInProgress = FALSE;                           // 清除操作进行中标志
    }

    if(pMsg->method == ATT_MTU_UPDATED_EVENT)                         // 如果是MTU更新事件
    {
        PRINT("MTU: %x\n", pMsg->msg.mtuEvt.MTU);                    // 打印新的MTU值
    }

    if((pMsg->method == ATT_READ_RSP) ||                             // 如果是读取响应
       ((pMsg->method == ATT_ERROR_RSP) &&                           // 或者是错误响应
        (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))             // 且是读取请求的错误
    {
        if(pMsg->method == ATT_ERROR_RSP)                            // 如果是错误响应
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;             // 获取错误码

            PRINT("Read Error: %x\n", status);                        // 打印读取错误
        }
        else
        {
            // After a successful read, display the read value         // 成功读取后显示读取的值
//            PRINT("Read rsp: %x\n", *pMsg->msg.readRsp.pValue);
//          PRINT("len = %d, Read rsp: ", pMsg->msg.readRsp.len);
//          for( i = 0; i < pMsg->msg.readRsp.len; i++){
//            PRINT("%02x ", pMsg->msg.readRsp.pValue[i]);
//          }PRINT("\n");
        }
//        tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, DEFAULT_WRITE_CCCD_DELAY);
//        tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY);
        centralProcedureInProgress = FALSE;                          // 清除操作进行中标志
    }
    else if((pMsg->method == ATT_WRITE_RSP) ||                      // 如果是写入响应
            ((pMsg->method == ATT_ERROR_RSP) &&                      // 或者是错误响应
             (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ)))      // 且是写入请求的错误
    {
        if(pMsg->method == ATT_ERROR_RSP)                           // 如果是错误响应
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;            // 获取错误码

            PRINT("Write Error: %x\n", status);                      // 打印写入错误
        }
        else
        {
            // Write success                                         // 写入成功
            PRINT("Write success \n");                              // 打印写入成功
        }

        centralProcedureInProgress = FALSE;                         // 清除操作进行中标志
    }
    else if(pMsg->method == ATT_HANDLE_VALUE_NOTI)                 // 如果是特征值通知
    {
//        PRINT("Receive noti: %x\n", *pMsg->msg.handleValueNoti.pValue);
      PRINT("Noti: ");                                             // 打印通知值
      for( i = 0; i < pMsg->msg.handleValueNoti.len; i++){         // 循环打印每个字节
        PRINT("%02x ", pMsg->msg.handleValueNoti.pValue[i]);       // 以16进制格式打印
      }PRINT("\n");

    }
    else if(centralDiscState != BLE_DISC_STATE_IDLE)               // 如果不是发现空闲状态
    {
        centralGATTDiscoveryEvent(pMsg);                           // 处理GATT发现事件
    }
    GATT_bm_free(&pMsg->msg, pMsg->method);                       // 释放消息内存
}

/*********************************************************************
 * @fn      centralRssiCB
 *
 * @brief   RSSI callback.                                          // RSSI回调函数
 *
 * @param   connHandle - connection handle                          // connHandle - 连接句柄
 * @param   rssi - RSSI                                            // rssi - RSSI值
 *
 * @return  none                                                    // 无返回值
 */
static void centralRssiCB(uint16_t connHandle, int8_t rssi)
{
    PRINT("RSSI : -%d dB \n", -rssi);                              // 打印RSSI值
}

/*********************************************************************
 * @fn      centralHciMTUChangeCB
 *
 * @brief   MTU changed callback.                                   // MTU变更回调函数
 *
 * @param   maxTxOctets - Max tx octets                            // maxTxOctets - 最大发送字节数
 * @param   maxRxOctets - Max rx octets                            // maxRxOctets - 最大接收字节数
 *
 * @return  none                                                    // 无返回值
 */
static void centralHciMTUChangeCB(uint16_t connHandle, uint16_t maxTxOctets, uint16_t maxRxOctets)
{
    attExchangeMTUReq_t req;                                        // MTU交换请求结构体

    req.clientRxMTU = maxRxOctets;                                 // 设置客户端接收MTU
    GATT_ExchangeMTU(connHandle, &req, centralTaskId);             // 发起MTU交换请求
    PRINT("exchange mtu:%d\n", maxRxOctets);                       // 打印交换的MTU值
    centralProcedureInProgress = TRUE;                             // 设置操作进行中标志
}

/*********************************************************************
 * @fn      centralEventCB
 *
 * @brief   Central event callback function.                        // 中央设备事件回调函数
 *
 * @param   pEvent - pointer to event structure                     // pEvent - 事件结构体指针
 *
 * @return  none                                                    // 无返回值
 */
static void centralEventCB(gapRoleEvent_t *pEvent)
{
    switch(pEvent->gap.opcode)                                      // 根据事件操作码处理
    {
        case GAP_DEVICE_INIT_DONE_EVENT:                           // 设备初始化完成事件
        {
            PRINT("Discovering...\n");                              // 打印开始发现设备
            GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,   // 开始设备发现
                                          DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                          DEFAULT_DISCOVERY_WHITE_LIST);
        }
        break;

        case GAP_DEVICE_INFO_EVENT:                                // 设备信息事件
        {
            // Add device to list                                   // 添加设备到列表
            centralAddDeviceInfo(pEvent->deviceInfo.addr, pEvent->deviceInfo.addrType);
        }
        break;

        case GAP_DEVICE_DISCOVERY_EVENT:                           // 设备发现事件
        {
            uint8_t i;

            // See if peer device has been discovered                // 检查是否发现目标设备
            for(i = 0; i < centralScanRes; i++)
            {
                if(tmos_memcmp(PeerAddrDef, centralDevList[i].addr, B_ADDR_LEN))
                    break;
            }

            // Peer device not found                                // 未找到目标设备
            if(i == centralScanRes)
            {
                PRINT("Device not found...\n");                     // 打印未找到设备
                centralScanRes = 0;
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE, // 重新开始设备发现
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
                PRINT("Discovering...\n");
            }

            // Peer device found                                    // 找到目标设备
            else
            {
                PRINT("Device found...\n");                         // 打印找到设备
                GAPRole_CentralEstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE, // 建立连接
                                             DEFAULT_LINK_WHITE_LIST,
                                             centralDevList[i].addrType,
                                             centralDevList[i].addr);

                // Start establish link timeout event                // 启动建立连接超时事件
                tmos_start_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT, ESTABLISH_LINK_TIMEOUT);
                PRINT("Connecting...\n");                           // 打印正在连接
            }
        }
        break;

        case GAP_LINK_ESTABLISHED_EVENT:
        {
            tmos_stop_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT);
            if(pEvent->gap.hdr.status == SUCCESS)
            {
                centralState = BLE_STATE_CONNECTED;
                centralConnHandle = pEvent->linkCmpl.connectionHandle;
                centralProcedureInProgress = TRUE;

                // Initiate service discovery
                tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, DEFAULT_SVC_DISCOVERY_DELAY);

                // See if initiate connect parameter update
                if(centralParamUpdate)
                {
                    tmos_start_task(centralTaskId, START_PARAM_UPDATE_EVT, DEFAULT_PARAM_UPDATE_DELAY);
                }
                // See if initiate phy update
                if(centralPhyUpdate)
                {
                    tmos_start_task(centralTaskId, START_PHY_UPDATE_EVT, DEFAULT_PHY_UPDATE_DELAY);
                }
                // See if start RSSI polling
                if(centralRssi)
                {
                    tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD);
                }

                PRINT("Connected...\n");
            }
            else
            {
                PRINT("Connect Failed...Reason:%X\n", pEvent->gap.hdr.status);
                PRINT("Discovering...\n");
                centralScanRes = 0;
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
            }
        }
        break;

        case GAP_LINK_TERMINATED_EVENT:
        {
            centralState = BLE_STATE_IDLE;
            centralConnHandle = GAP_CONNHANDLE_INIT;
            centralDiscState = BLE_DISC_STATE_IDLE;
            centralCharHdl = 0;
            centralScanRes = 0;
            centralProcedureInProgress = FALSE;
            tmos_stop_task(centralTaskId, START_READ_RSSI_EVT);
            PRINT("Disconnected...Reason:%x\n", pEvent->linkTerminate.reason);
            PRINT("Discovering...\n");
            GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                          DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                          DEFAULT_DISCOVERY_WHITE_LIST);
        }
        break;

        case GAP_LINK_PARAM_UPDATE_EVENT:
        {
            PRINT("Param Update...\n");
        }
        break;

        case GAP_PHY_UPDATE_EVENT:
        {
            PRINT("PHY Update...\n");
        }
        break;

        case GAP_EXT_ADV_DEVICE_INFO_EVENT:
        {
            // Display device addr
            PRINT("Recv ext adv \n");
            // Add device to list
            centralAddDeviceInfo(pEvent->deviceExtAdvInfo.addr, pEvent->deviceExtAdvInfo.addrType);
        }
        break;

        case GAP_DIRECT_DEVICE_INFO_EVENT:
        {
            // Display device addr
            PRINT("Recv direct adv \n");
            // Add device to list
            centralAddDeviceInfo(pEvent->deviceDirectInfo.addr, pEvent->deviceDirectInfo.addrType);
        }
        break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      pairStateCB
 *
 * @brief   Pairing state callback.                                 // 配对状态回调函数
 *
 * @return  none                                                    // 无返回值
 */
static void centralPairStateCB(uint16_t connHandle, uint8_t state, uint8_t status)
{
    if(state == GAPBOND_PAIRING_STATE_STARTED)                      // 如果是配对开始状态
    {
        PRINT("Pairing started:%d\n", status);                      // 打印配对开始
    }
    else if(state == GAPBOND_PAIRING_STATE_COMPLETE)               // 如果是配对完成状态
    {
        if(status == SUCCESS)                                       // 如果配对成功
        {
            PRINT("Pairing success\n");                             // 打印配对成功
        }
        else
        {
            PRINT("Pairing fail\n");                                // 打印配对失败
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BONDED)                 // 如果是已绑定状态
    {
        if(status == SUCCESS)                                       // 如果绑定成功
        {
            PRINT("Bonding success\n");                             // 打印绑定成功
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BOND_SAVED)             // 如果是绑定保存状态
    {
        if(status == SUCCESS)                                       // 如果保存成功
        {
            PRINT("Bond save success\n");                           // 打印绑定保存成功
        }
        else
        {
            PRINT("Bond save failed: %d\n", status);                // 打印绑定保存失败
        }
    }
}

/*********************************************************************
 * @fn      centralPasscodeCB
 *
 * @brief   Passcode callback.                                      // 密码回调函数
 *
 * @return  none                                                    // 无返回值
 */
static void centralPasscodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                              uint8_t uiInputs, uint8_t uiOutputs)
{
    uint32_t passcode;                                              // 密码变量

    // Create random passcode                                       // 创建随机密码
    passcode = tmos_rand();
    passcode %= 1000000;                                           // 限制密码为6位数
    // Display passcode to user                                    // 显示密码给用户
    if(uiOutputs != 0)
    {
        PRINT("Passcode:%06d\n", (int)passcode);                   // 打印6位密码
    }
    // Send passcode response                                      // 发送密码响应
    GAPBondMgr_PasscodeRsp(connectionHandle, SUCCESS, passcode);
}

/*********************************************************************
 * @fn      centralStartDiscovery
 *
 * @brief   Start service discovery.                               // 开始服务发现
 *
 * @return  none                                                   // 无返回值
 */
static void centralStartDiscovery(void)
{
    uint8_t uuid[ATT_BT_UUID_SIZE] = {LO_UINT16(SIMPLEPROFILE_SERV_UUID),
                                      HI_UINT16(SIMPLEPROFILE_SERV_UUID)}; // 服务UUID

    // Initialize cached handles                                    // 初始化缓存的句柄
    centralSvcStartHdl = centralSvcEndHdl = centralCharHdl = 0;

    centralDiscState = BLE_DISC_STATE_SVC;                         // 设置发现状态为服务发现

    // Discovery simple BLE service                                // 发现简单BLE服务
    GATT_DiscPrimaryServiceByUUID(centralConnHandle,
                                  uuid,
                                  ATT_BT_UUID_SIZE,
                                  centralTaskId);
}

/*********************************************************************
 * @fn      centralGATTDiscoveryEvent
 *
 * @brief   Process GATT discovery event                           // 处理GATT发现事件
 *
 * @return  none                                                   // 无返回值
 */
static void centralGATTDiscoveryEvent(gattMsgEvent_t *pMsg)
{
    attReadByTypeReq_t req;                                        // 读取请求结构体
    if(centralDiscState == BLE_DISC_STATE_SVC)                     // 如果是服务发现状态
    {
        // Service found, store handles                            // 找到服务，存储句柄
        if(pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
           pMsg->msg.findByTypeValueRsp.numInfo > 0)
        {
            centralSvcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0); // 保存服务起始句柄
            centralSvcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0); // 保存服务结束句柄

            // Display Profile Service handle range                 // 显示配置文件服务句柄范围
            PRINT("Found Profile Service handle : %x ~ %x \n", centralSvcStartHdl, centralSvcEndHdl);
        }
        // If procedure complete                                   // 如果程序完成
        if((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            if(centralSvcStartHdl != 0)                            // 如果找到服务
            {
                // Discover characteristic                         // 发现特征
                centralDiscState = BLE_DISC_STATE_CHAR;            // 设置状态为特征发现
                req.startHandle = centralSvcStartHdl;              // 设置起始句柄
                req.endHandle = centralSvcEndHdl;                  // 设置结束句柄
                req.type.len = ATT_BT_UUID_SIZE;                   // 设置UUID长度
                req.type.uuid[0] = LO_UINT16(SIMPLEPROFILE_CHAR1_UUID); // 设置特征UUID
                req.type.uuid[1] = HI_UINT16(SIMPLEPROFILE_CHAR1_UUID);

//                GATT_ReadUsingCharUUID(centralConnHandle, &req, centralTaskId);
                GATT_DiscAllChars(centralConnHandle, centralSvcStartHdl, centralSvcEndHdl, centralTaskId); // 发现所有特征
            }
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CHAR)              // 如果是特征发现状态
    {
        // Characteristic found, store handle                      // 找到特征，存储句柄
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            centralCharHdl = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[0], // 构建特征句柄
                                          pMsg->msg.readByTypeRsp.pDataList[1]);

            // Start do read or write                             // 开始读写操作
            tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY);

            // Display Characteristic 1 handle                     // 显示特征1句柄
            PRINT("Found Characteristic 1 handle : %x \n", centralCharHdl);
        }
        if((pMsg->method == ATT_READ_BY_TYPE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            // Discover characteristic                            // 发现特征
            centralDiscState = BLE_DISC_STATE_CCCD;               // 设置状态为CCCD发现
            req.startHandle = centralSvcStartHdl;                 // 设置起始句柄
            req.endHandle = centralSvcEndHdl;                     // 设置结束句柄
            req.type.len = ATT_BT_UUID_SIZE;                      // 设置UUID长度
            req.type.uuid[0] = LO_UINT16(GATT_CLIENT_CHAR_CFG_UUID); // 设置CCCD UUID
            req.type.uuid[1] = HI_UINT16(GATT_CLIENT_CHAR_CFG_UUID);

            GATT_ReadUsingCharUUID(centralConnHandle, &req, centralTaskId); // 读取CCCD
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CCCD)             // 如果是CCCD发现状态
    {
        // Characteristic found, store handle                     // 找到特征，存储句柄
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            centralCCCDHdl = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[0], // 构建CCCD句柄
                                          pMsg->msg.readByTypeRsp.pDataList[1]);
            centralProcedureInProgress = FALSE;                   // 清除操作进行中标志

            // Start do write CCCD                                // 开始写CCCD
            tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, DEFAULT_WRITE_CCCD_DELAY);

            // Display Characteristic 1 handle                    // 显示CCCD句柄
            PRINT("Found client characteristic configuration handle : %x \n", centralCCCDHdl);
        }
        centralDiscState = BLE_DISC_STATE_IDLE;                  // 设置状态为空闲
    }
}

/*********************************************************************
 * @fn      centralAddDeviceInfo
 *
 * @brief   Add a device to the device discovery result list       // 添加设备到设备发现结果列表
 *
 * @return  none                                                   // 无返回值
 */
static void centralAddDeviceInfo(uint8_t *pAddr, uint8_t addrType)
{
    uint8_t i;

    // If result count not at max                                  // 如果结果数量未达到最大值
    if(centralScanRes < DEFAULT_MAX_SCAN_RES)
    {
        // Check if device is already in scan results              // 检查设备是否已在扫描结果中
        for(i = 0; i < centralScanRes; i++)
        {
            if(tmos_memcmp(pAddr, centralDevList[i].addr, B_ADDR_LEN))
            {
                return;
            }
        }
        // Add addr to scan result list                           // 添加地址到扫描结果列表
        tmos_memcpy(centralDevList[centralScanRes].addr, pAddr, B_ADDR_LEN);
        centralDevList[centralScanRes].addrType = addrType;
        // Increment scan result count                            // 增加扫描结果计数
        centralScanRes++;
        // Display device addr                                    // 显示设备地址
//        PRINT("Device %d - Addr %x %x %x %x %x %x \n", centralScanRes,
//              centralDevList[centralScanRes - 1].addr[0],
//              centralDevList[centralScanRes - 1].addr[1],
//              centralDevList[centralScanRes - 1].addr[2],
//              centralDevList[centralScanRes - 1].addr[3],
//              centralDevList[centralScanRes - 1].addr[4],
//              centralDevList[centralScanRes - 1].addr[5]);
    }
}

/************************ endfile @ central **************************/
