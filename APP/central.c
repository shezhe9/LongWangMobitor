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
#include "ulog_buffer.h"                                             // ulog 日志系统  
#include "oled_display.h"                                            // OLED显示  
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
#define DEFAULT_SCAN_DURATION               1600                     // 扫描持续时间，单位0.625ms (1秒)

// Connection min interval in 1.25ms
#define DEFAULT_MIN_CONNECTION_INTERVAL     80                       // 最小连接间隔，单位1.25ms (100ms) - 更保守设置

// Connection max interval in 1.25ms
#define DEFAULT_MAX_CONNECTION_INTERVAL     100                      // 最大连接间隔，单位1.25ms (125ms) - 标准设置

// Connection supervision timeout in 10ms
#define DEFAULT_CONNECTION_TIMEOUT          400                      // 连接监督超时，单位10ms (4秒) - 更长超时

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
#define DEFAULT_UPDATE_MIN_CONN_INTERVAL    8                       // 更新连接参数的最小间隔 (10ms) - 完全在从机期望范围内

// Maximum connection interval (units of 1.25ms)
#define DEFAULT_UPDATE_MAX_CONN_INTERVAL    12                      // 更新连接参数的最大间隔 (15ms) - 完全在从机期望范围内

// Slave latency to use parameter update
#define DEFAULT_UPDATE_SLAVE_LATENCY        0                       // 从机延迟参数

// Supervision timeout value (units of 10ms)
#define DEFAULT_UPDATE_CONN_TIMEOUT         300                     // 连接超时值，单位10ms (3秒) - 匹配从机期望

// Default passcode                                                 
#define DEFAULT_PASSCODE                    0                       // 默认配对密码

// Default GAP pairing mode
#define DEFAULT_PAIRING_MODE                GAPBOND_PAIRING_MODE_WAIT_FOR_REQ // 允许配对请求

// Default MITM mode (TRUE to require passcode or OOB when pairing) 
#define DEFAULT_MITM_MODE                   FALSE                   // 默认不启用MITM保护

// Default bonding mode, TRUE to bond, max bonding 6 devices        
#define DEFAULT_BONDING_MODE                TRUE                    // 默认启用绑定模式，最多绑定6个设备

// Default GAP bonding I/O capabilities
#define DEFAULT_IO_CAPABILITIES             GAPBOND_IO_CAP_NO_INPUT_NO_OUTPUT // 无输入无输出（Just Works配对）

// Default service discovery timer delay in 0.625ms
#define DEFAULT_SVC_DISCOVERY_DELAY         1600                    // 默认服务发现延时 (1秒) - 给连接稳定时间
// Default parameter update delay in 0.625ms                        
#define DEFAULT_PARAM_UPDATE_DELAY          3200                    // 默认参数更新延时

// Default phy update delay in 0.625ms                              
#define DEFAULT_PHY_UPDATE_DELAY            2400                    // 默认PHY更新延时

// Default read or write timer delay in 0.625ms                     
#define DEFAULT_READ_OR_WRITE_DELAY         1600                    // 默认读写操作延时

// Default write CCCD delay in 0.625ms                              
//#define DEFAULT_WRITE_CCCD_DELAY            320                     // 默认写CCCD延时
#define DEFAULT_WRITE_CCCD_DELAY            400                     // DragonK 需要这么多才能成功

// Establish link timeout in 0.625ms
#define ESTABLISH_LINK_TIMEOUT              1600                    // 建立连接超时时间

// 服务发现重试机制
#define MAX_SVC_DISCOVERY_RETRIES           3                       // 最大服务发现重试次数
#define SVC_DISCOVERY_RETRY_DELAY           800                     // 服务发现重试延时（0.625ms单位）

// 连接心跳机制 - 暂时禁用，专注解决连接问题
// #define HEARTBEAT_INTERVAL                 8000                    // 心跳间隔，单位0.625ms (5秒)
// #define HEARTBEAT_DATA_SIZE                8                       // 心跳数据大小

// Application states（已移至central.h中定义）

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
uint8_t centralTaskId;                                               // 中央设备任务ID（供外部访问）

// Number of scan results                                            
static uint8_t centralScanRes;                                       // 扫描结果数量

// Scan result list                                                  
static gapDevRec_t centralDevList[DEFAULT_MAX_SCAN_RES];            // 扫描结果列表

// 目标设备候选列表（智能信号优选）
#define MAX_CANDIDATES    5  // 最多保存5个候选设备

typedef struct {
    uint8_t  addr[B_ADDR_LEN];  // 设备地址
    uint8_t  addrType;          // 地址类型
    int8_t   rssi;              // 信号强度
    uint8_t  nameIndex;         // 设备名称索引 (1=HID-LongWang, 2=DragonK)
    uint8_t  valid;             // 是否有效
} candidateDevice_t;

static candidateDevice_t candidates[MAX_CANDIDATES];      // 候选设备列表
static uint8_t candidateCount = 0;                        // 当前候选设备数量

// 目标设备名称（替换原来的硬编码MAC地址）                                               
static uint8_t targetDeviceName1[] = TARGET_DEVICE_NAME_1; // 目标设备名称1
static uint8_t targetDeviceName2[] = TARGET_DEVICE_NAME_2; // 目标设备名称2
static uint8_t connectedDeviceName[32] = {0};             // 当前连接的设备名称
static uint8_t targetDeviceFound = FALSE;                 // 是否找到目标设备
static uint8_t connectionFailCount = 0;                   // 连接失败计数器
static uint8_t svcDiscoveryRetryCount = 0;                // 服务发现重试计数器

// 新增：自动重连控制变量
static uint8_t autoReconnectEnabled = TRUE;               // 是否启用自动重连功能
static uint8_t userTriggeredReconnect = FALSE;            // 用户主动触发的重连标志

// RSSI polling state                                                
static uint8_t centralRssi = FALSE;                                  // RSSI轮询状态（禁用以避免连接问题）

// Parameter update state                                            
static uint8_t centralParamUpdate = FALSE;                          // 参数更新状态（禁用以避免连接问题）

// Phy update state                                                  
static uint8 centralPhyUpdate = FALSE;                              // PHY更新状态

// Connection handle of current connection（供外部访问）                           
uint16_t centralConnHandle = GAP_CONNHANDLE_INIT;                   // 当前连接句柄

// Application state（供外部访问）
uint8_t centralState = BLE_STATE_IDLE;                              // 应用状态

// Connection establishment flag (for stability checking)
static uint8_t connectionJustEstablished = 0;                       // 连接刚建立标志


// Discovery state
static uint8_t centralDiscState = BLE_DISC_STATE_IDLE;              // 发现状态

// Discovered service start and end handle                           
static uint16_t centralSvcStartHdl = 0;                             // 发现的服务起始句柄
static uint16_t centralSvcEndHdl = 0;                               // 发现的服务结束句柄

// Discovered characteristic handles                                  
static uint16_t centralNotifyCharHdl = 0;                           // AE02通知特征句柄
uint16_t centralWriteCharHdl = 0;                                   // AE10写特征句柄（供外部访问）

// Discovered Client Characteristic Configuration handle              
static uint16_t centralCCCDHdl = 0;                                 // 发现的客户端特征配置句柄

// 保持向后兼容性的旧变量（指向写特征）
static uint16_t centralCharHdl = 0;                                 // 发现的特征句柄（兼容旧代码）

// Value to write                                                    
static uint8_t centralCharVal = 0x5A;                               // 要写入的值

// Value read/write toggle                                           
static uint8_t centralDoWrite = TRUE;                               // 读/写切换标志

// GATT read/write procedure state（供外部访问）                                   
uint8_t centralProcedureInProgress = FALSE;                         // GATT读/写过程状态

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

// 候选设备管理函数
static void centralInitCandidates(void);                            // 初始化候选列表
static void centralAddCandidate(uint8_t *addr, uint8_t addrType, int8_t rssi, uint8_t nameIndex); // 添加候选设备

// 版本号管理
static uint16_t deviceVersion = 0;                                  // 设备版本号
static void parseDeviceVersion(uint8_t *data, uint8_t len);         // 解析设备版本号
static candidateDevice_t* centralGetBestCandidate(void);            // 获取信号最强的候选设备

/*********************************************************************
 * PROFILE CALLBACKS                                                 // 配置文件回调
 */

// GAP Role Callbacks                                                // GAP角色回调结构体
static gapCentralRoleCB_t centralRoleCB = {
    centralRssiCB,        // RSSI callback                          // RSSI回调
    centralEventCB,       // Event callback                         // 事件回调
    NULL                  // MTU change callback (disabled)        // MTU变更回调（禁用）
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
    
    // 初始化状态变量
    centralState = BLE_STATE_IDLE;
    centralConnHandle = GAP_CONNHANDLE_INIT;
    centralDiscState = BLE_DISC_STATE_IDLE;
    centralScanRes = 0;
    targetDeviceFound = FALSE;
    centralProcedureInProgress = FALSE;
    centralNotifyCharHdl = 0;
    centralWriteCharHdl = 0;
    centralCharHdl = 0;
    centralCCCDHdl = 0;
    autoReconnectEnabled = TRUE;  // 默认启用自动重连
    centralInitCandidates();      // 初始化候选设备列表
    
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
uint8_t send_data_index = 1;
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
    // 调试：打印接收到的事件（排除高频的SYS_EVENT_MSG）
    if(events != SYS_EVENT_MSG && events != 0)
    {
        //uinfo(">>> Central_ProcessEvent: taskId=%d, events=0x%04X <<<\n", task_id, events);
    }
    
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
            uinfo("Connection timeout! Terminating connection attempt...\n");
            GAPRole_TerminateLink(INVALID_CONNHANDLE);                    // 终止连接
            
            // 重置状态并重新开始搜索
            centralState = BLE_STATE_IDLE;
            centralConnHandle = GAP_CONNHANDLE_INIT;
            centralScanRes = 0;
            targetDeviceFound = FALSE;
            centralProcedureInProgress = FALSE;
            
            // 只有在启用自动重连时才重新开始搜索
            if(autoReconnectEnabled == TRUE)
            {
                uinfo("Restarting device discovery after timeout...\n");
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
            }
            else
            {
                uinfo("Auto reconnect disabled, not restarting discovery after timeout.\n");
            }
            
            return (events ^ ESTABLISH_LINK_TIMEOUT_EVT);
        }

    if(events & START_SVC_DISCOVERY_EVT)                              // 如果是开始服务发现事件
    {
        // start service discovery                                     // 开始服务发现
        uinfo("Starting service discovery (attempt %d/%d)...\n", svcDiscoveryRetryCount + 1, MAX_SVC_DISCOVERY_RETRIES);
        centralStartDiscovery();
        return (events ^ START_SVC_DISCOVERY_EVT);
    }

    if(events & START_PARAM_UPDATE_EVT)                               // 如果是开始参数更新事件
    {
        // start connect parameter update                              // 开始连接参数更新
        uinfo("Updating connection parameters: min=%d, max=%d, timeout=%d\n",
              DEFAULT_UPDATE_MIN_CONN_INTERVAL, DEFAULT_UPDATE_MAX_CONN_INTERVAL, DEFAULT_UPDATE_CONN_TIMEOUT);

        bStatus_t status = GAPRole_UpdateLink(centralConnHandle,
                           DEFAULT_UPDATE_MIN_CONN_INTERVAL,           // 使用默认的最小连接间隔
                           DEFAULT_UPDATE_MAX_CONN_INTERVAL,           // 使用默认的最大连接间隔
                           DEFAULT_UPDATE_SLAVE_LATENCY,               // 使用默认的从机延迟
                           DEFAULT_UPDATE_CONN_TIMEOUT);               // 使用默认的连接超时

        if(status == SUCCESS)
        {
            uinfo("Connection parameter update initiated successfully\n");
            // 参数更新启动后，延迟一段时间再开始服务发现
            tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, DEFAULT_SVC_DISCOVERY_DELAY);
        }
        else
        {
            uinfo("Connection parameter update failed: 0x%02X, proceeding with service discovery\n", status);
            // 如果参数更新失败，仍然进行服务发现
            tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, DEFAULT_SVC_DISCOVERY_DELAY);
        }
        return (events ^ START_PARAM_UPDATE_EVT);
    }

    if(events & START_PHY_UPDATE_EVT)                                 // 如果是开始PHY更新事件
    {
        // start phy update                                           // 开始PHY更新
        uinfo("PHY Update %x...\n", GAPRole_UpdatePHY(centralConnHandle, 0, 
                    GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, GAP_PHY_OPTIONS_NOPRE));

        return (events ^ START_PHY_UPDATE_EVT);
    }

    if(events & START_READ_OR_WRITE_EVT)                              // 如果是开始读写事件（现在只做写操作）
    {
        if(centralProcedureInProgress == FALSE)                       // 如果当前没有正在进行的操作
        {
            // 只执行写操作，读操作已注释
            // Do a write                                             // 执行写操作
            attWriteReq_t req;

            req.cmd = FALSE;                                          // 不是命令
            req.sig = FALSE;                                          // 不带签名
            req.handle = centralWriteCharHdl;                         // 设置AE10写特征句柄

            req.len = 1;                                              // 写入长度为1
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // 分配内存
            uinfo("WRITE_handle = 0x%04X\n", req.handle);             // 打印句柄值
            if(req.pValue != NULL)                                    // 如果内存分配成功
            {
                *req.pValue = centralCharVal;                         // 设置写入值

                if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // 写入AE10特征值
                {
                    centralProcedureInProgress = TRUE;                // 设置操作进行中标志
                    uinfo("Writing value 0x%02X to AE10 write characteristic\n", centralCharVal);
                }
                else
                {
                    GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);   // 释放内存
                    uinfo("Write failed\n");
                }
            }
            
            /* 读操作已注释
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
            */
        }
        return (events ^ START_READ_OR_WRITE_EVT);
    }

    if(events & START_WRITE_CCCD_EVT)                                   // 如果是开始写CCCD事件
    {
        if(centralProcedureInProgress == FALSE)                        // 如果当前没有正在进行的操作
        {
            // Do a write - 使用Write Command (无需响应)避免阻塞
            attWriteReq_t req;

            req.cmd = TRUE;                                            // 使用Write Command（无需响应）
            req.sig = FALSE;                                           // 不带签名
            req.handle = centralCCCDHdl;                               // 设置CCCD句柄
            req.len = 2;                                               // 写入长度为2
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0); // 分配内存
            if(req.pValue != NULL)                                     // 如果内存分配成功
            {
                req.pValue[0] = 1;                                     // 启用通知
                req.pValue[1] = 0;

                bStatus_t status = GATT_WriteNoRsp(centralConnHandle, &req); // 使用Write Command
                if(status == SUCCESS) // 写入CCCD值
                {
                    uinfo("CCCD write request sent successfully (no response required)\n");
                    // 注意：Write Command不需要等待响应，所以不设置centralProcedureInProgress

                    // 延迟启动初始化数据发送
                    tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 800); // 500ms后发送初始化数据
                }
                else
                {
                    uinfo("Failed to send CCCD write request: 0x%02X\n", status);

                    // 即使CCCD写入失败，也尝试发送初始化数据
                    tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 800);
                }
                GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);   // 释放内存
            }
            else
            {
                uinfo("Failed to allocate memory for CCCD write\n");

                // 即使CCCD内存分配失败，也尝试发送初始化数据
                tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 800);
            }
        }
        else
        {
            uinfo("GATT procedure in progress, retrying CCCD write in 500ms...\n");
            // 重试CCCD写入
            tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, 800); // 500ms后重试
        }
        return (events ^ START_WRITE_CCCD_EVT);
    }

    if(events & START_READ_RSSI_EVT)                                  // 如果是开始读取RSSI事件
    {
        GAPRole_ReadRssiCmd(centralConnHandle);                       // 读取RSSI值
        tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD); // 启动下一次RSSI读取
        return (events ^ START_READ_RSSI_EVT);
    }

    if(events & START_SEND_TEST_DATA_EVT)                             // 如果是发送测试数据事件
    {
        if(centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT && centralWriteCharHdl != 0)
        {
            // 检查是否有其他GATT操作正在进行
            if(centralProcedureInProgress == TRUE)
            {
                uinfo("GATT procedure in progress, retrying test data send in 100ms...\n");
                tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 100); // 100ms后重试
                return (events ^ START_SEND_TEST_DATA_EVT);
            }
            
            // 发送指定的20字节数据到AE10写特征
            uinfo("Sending custom command to AE10 write characteristic handle: 0x%04X\n", centralWriteCharHdl);
            
            attWriteReq_t req;
            req.cmd = TRUE;                                           // 使用Write Command（无需响应）
            req.sig = FALSE;                                          // 不带签名
            req.handle = centralWriteCharHdl;                         // 设置AE10写特征句柄
            req.len = 20;                                             // 写入长度为20字节
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
            
            if(req.pValue != NULL)
            {
                // 填充指定的数据：83 00 01 01 0a 02 00 2d 00 00 02 00 37 00 00 00 00 00 00 00
                req.pValue[0]  = 0x83;
                req.pValue[1]  = 0x00;
                req.pValue[2]  = 0x01;
                req.pValue[3]  = 0x01;
                req.pValue[4]  = 0x0A;
                req.pValue[5]  = 0x02;
                req.pValue[6]  = 0x00;
                req.pValue[7]  = 0x2D;
                req.pValue[8]  = 0x00;
                req.pValue[9]  = 0x00;
                req.pValue[10] = 0x02;
                req.pValue[11] = 0x00;
                req.pValue[12] = 0x37;
                req.pValue[13] = 0x00;
                req.pValue[14] = 0x00;
                req.pValue[15] = 0x00;
                req.pValue[16] = 0x00;
                req.pValue[17] = 0x00;
                req.pValue[18] = 0x00;
                req.pValue[19] = 0x00;
                
                // 使用ulog_array_to_hex一次性打印，避免多条日志
                ulog_array_to_hex("Sending custom command", req.pValue, 20);
                
                bStatus_t status = GATT_WriteNoRsp(centralConnHandle, &req);  // 使用Write Command
                if(status == SUCCESS)
                {
                    uinfo("Custom command sent successfully to AE10!\n");
                }
                else
                {
                    uinfo("Failed to send custom command, status: 0x%02X\n", status);
                    GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
                }
            }
            else
            {
                uinfo("Failed to allocate memory for custom command\n");
            }
        }
        else
        {
            uinfo("Cannot send custom command:\n");
            uinfo("  State: %d (expected: %d - BLE_STATE_CONNECTED)\n", centralState, BLE_STATE_CONNECTED);
            uinfo("  ConnHandle: 0x%04X (expected: != 0xFFFE)\n", centralConnHandle);
            uinfo("  AE10 WriteHandle: 0x%04X (expected: != 0)\n", centralWriteCharHdl);
        }
        return (events ^ START_SEND_TEST_DATA_EVT);
    }

    if(events & START_SEND_INIT_DATA_EVT)                            // 如果是发送初始化数据事件
    {
        if(centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT && centralWriteCharHdl != 0)
        {
            // 检查是否有其他GATT操作正在进行
            if(centralProcedureInProgress == TRUE)
            {
                uinfo("GATT procedure in progress, retrying init data send in 100ms...\n");
                tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 100); // 100ms后重试
                return (events ^ START_SEND_INIT_DATA_EVT);
            }
            
            // 发送初始化数据：0x76 0x00 0x01 0x01
            uinfo("Sending initialization data to AE10 write characteristic handle: 0x%04X\n", centralWriteCharHdl);
            
            attWriteReq_t req;
            req.cmd = TRUE;                                           // 使用Write Command（无需响应）
            req.sig = FALSE;                                          // 不带签名
            req.handle = centralWriteCharHdl;                         // 设置AE10写特征句柄
            req.len = 4;                                              // 写入长度为4字节
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
            
            if(req.pValue != NULL)
            {
                // 填充初始化数据：0x75 0x00 0x01 0x01
                req.pValue[0] = 0x75;
                req.pValue[1] = 0x00;
                req.pValue[2] = 0x01;
                req.pValue[3] = 0x01;
                
                // 打印要发送的数据
                uinfo("Sending init data: 0x%02X 0x%02X 0x%02X 0x%02X\n", 
                      req.pValue[0], req.pValue[1], req.pValue[2], req.pValue[3]);
                
                bStatus_t status = GATT_WriteNoRsp(centralConnHandle, &req);  // 使用Write Command
                if(status == SUCCESS)
                {
                    uinfo("Initialization data sent successfully to AE10!\n");

                    // 心跳机制暂时禁用，专注解决连接问题
                    // uinfo("[连接维护] Starting heartbeat mechanism...\n");
                    // tmos_start_task(centralTaskId, START_HEARTBEAT_EVT, HEARTBEAT_INTERVAL);
                }
                else
                {
                    uinfo("Failed to send initialization data, status: 0x%02X\n", status);
                    GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);

                    // 心跳机制暂时禁用
                    // uinfo("[连接维护] Starting heartbeat mechanism (init data failed)...\n");
                    // tmos_start_task(centralTaskId, START_HEARTBEAT_EVT, HEARTBEAT_INTERVAL);
                }
            }
            else
            {
                uinfo("Failed to allocate memory for initialization data\n");
            }
        }
        else
        {
            uinfo("Cannot send initialization data:\n");
            uinfo("  State: %d (expected: %d - BLE_STATE_CONNECTED)\n", centralState, BLE_STATE_CONNECTED);
            uinfo("  ConnHandle: 0x%04X (expected: != 0xFFFE)\n", centralConnHandle);
            uinfo("  AE10 WriteHandle: 0x%04X (expected: != 0)\n", centralWriteCharHdl);
        }
        return (events ^ START_SEND_INIT_DATA_EVT);
    }

    if(events & STOP_AUTO_RECONNECT_EVT)                             // 如果是停止自动重连事件
    {
        uinfo("Stopping auto reconnect functionality...\n");
        autoReconnectEnabled = FALSE;                                // 禁用自动重连
        
        // 如果当前有连接，断开连接
        if(centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT)
        {
            uinfo("Disconnecting current BLE connection...\n");
            GAPRole_TerminateLink(centralConnHandle);
        }
        
        // 停止当前的扫描
        if(centralState == BLE_STATE_IDLE || centralState == BLE_STATE_CONNECTING)
        {
            uinfo("Stopping BLE discovery...\n");
            GAPRole_CentralCancelDiscovery();
        }
        
        // 停止所有相关的定时任务
        tmos_stop_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT);
        tmos_stop_task(centralTaskId, START_READ_RSSI_EVT);
        
        uinfo("Auto reconnect stopped. Device will not automatically search for BLE devices.\n");
        return (events ^ STOP_AUTO_RECONNECT_EVT);
    }

    if(events & START_AUTO_RECONNECT_EVT)                            // 如果是启动自动重连事件
    {
        uinfo("Starting auto reconnect functionality...\n");
        autoReconnectEnabled = TRUE;                                 // 启用自动重连
        
        // 先清除可能存在的重复事件
        tmos_stop_task(centralTaskId, DELAYED_DISCOVERY_RETRY_EVT);
        tmos_stop_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT);
        
        // 检查当前连接状态，避免状态冲突
        if(centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT)
        {
            // 已连接，需要先断开连接再重新扫描
            uinfo("Already connected, disconnecting first...\n");
            userTriggeredReconnect = TRUE;  // 设置用户触发标志
            GAPRole_TerminateLink(centralConnHandle);
            
            // 断开后会触发GAP_LINK_TERMINATED_EVENT，在那里会检查此标志
            // 所以这里直接返回，不继续执行
            return (events ^ START_AUTO_RECONNECT_EVT);
        }
        else if(centralState == BLE_STATE_CONNECTING)
        {
            // 正在连接中，取消当前连接尝试
            uinfo("Connection in progress, canceling...\n");
            GAPRole_TerminateLink(INVALID_CONNHANDLE);
        }
        else if(centralState != BLE_STATE_IDLE)
        {
            // 其他非空闲状态，先重置
            uinfo("Resetting from state %d to IDLE...\n", centralState);
        }
        
        // 重置连接状态
        centralState = BLE_STATE_IDLE;
        centralConnHandle = GAP_CONNHANDLE_INIT;
        centralDiscState = BLE_DISC_STATE_IDLE;
        centralScanRes = 0;
        targetDeviceFound = FALSE;
        centralProcedureInProgress = FALSE;
        connectionFailCount = 0;
        
        // 重置特征句柄
        centralNotifyCharHdl = 0;
        centralWriteCharHdl = 0;
        centralCharHdl = 0;
        centralCCCDHdl = 0;
        
        // 初始化候选设备列表
        centralInitCandidates();
        
        // 取消扫描（如果正在进行）
        GAPRole_CentralCancelDiscovery();
        
        uinfo("Restarting device discovery (%s / %s), will select strongest signal...\n", 
              TARGET_DEVICE_NAME_1, TARGET_DEVICE_NAME_2);
        GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                      DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                      DEFAULT_DISCOVERY_WHITE_LIST);
        
        return (events ^ START_AUTO_RECONNECT_EVT);
    }

    if(events & DELAYED_DISCOVERY_RETRY_EVT)                         // 延迟后重新发现设备事件
    {
        if(autoReconnectEnabled == TRUE && targetDeviceFound == FALSE)
        {
            // 初始化候选设备列表
            centralInitCandidates();
            
            GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                          DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                          DEFAULT_DISCOVERY_WHITE_LIST);
        }
        else
        {
            uinfo("Discovery retry cancelled (autoReconnect=%d, targetFound=%d)\n", 
                  autoReconnectEnabled, targetDeviceFound);
        }
        return (events ^ DELAYED_DISCOVERY_RETRY_EVT);
    }


    // 注意：断开连接后的延迟重连现在使用START_AUTO_RECONNECT_EVT事件（0x2000）
    // 不再需要DELAYED_RECONNECT_EVT（0x8000太大，TMOS不支持）

    // 心跳机制暂时禁用，专注解决连接问题
    // if(events & START_HEARTBEAT_EVT)                                   // 如果是心跳事件
    // {
    //     // ... 心跳处理代码 ...
    // }

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

            uinfo("Exchange MTU Error: %x\n", status);                 // 打印MTU交换错误
        }
        else
        {
            uinfo("MTU exchange completed successfully\n");
        }
        centralProcedureInProgress = FALSE;                           // 清除操作进行中标志
        
        // MTU交换完成后，如果服务发现正在等待，则重新触发
        if(centralDiscState == BLE_DISC_STATE_SVC && centralSvcStartHdl == 0)
        {
            uinfo("MTU exchange done, retrying service discovery...\n");
            tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, 800); // 500ms后重试服务发现（增加延时确保连接稳定）
        }
    }

    if(pMsg->method == ATT_MTU_UPDATED_EVENT)                         // 如果是MTU更新事件
    {
        uinfo("MTU: %x\n", pMsg->msg.mtuEvt.MTU);                    // 打印新的MTU值
    }

    if((pMsg->method == ATT_READ_RSP) ||                             // 如果是读取响应
       ((pMsg->method == ATT_ERROR_RSP) &&                           // 或者是错误响应
        (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))             // 且是读取请求的错误
    {
        if(pMsg->method == ATT_ERROR_RSP)                            // 如果是错误响应
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;             // 获取错误码

            uinfo("Read Error: %x\n", status);                        // 打印读取错误
        }
        else
        {
            // After a successful read, display the read value         // 成功读取后显示读取的值
//            uinfo("Read rsp: %x\n", *pMsg->msg.readRsp.pValue);
//          uinfo("len = %d, Read rsp: ", pMsg->msg.readRsp.len);
//          for( i = 0; i < pMsg->msg.readRsp.len; i++){
//            uinfo("%02x ", pMsg->msg.readRsp.pValue[i]);
//          }uinfo("\n");
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

            uinfo("Write Error: %x\n", status);                      // 打印写入错误
        }
        else
        {
            // Write success - CCCD写入成功
            if(centralDiscState == BLE_DISC_STATE_IDLE && centralWriteCharHdl != 0)
            {
                uinfo("Notifications enabled.\n");
                
                // 更新OLED显示 - 阶段7：启用通知
#ifdef ENABLE_OLED_DISPLAY
                OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 7, 0);
#endif
                
                // 自动发送初始化数据
                uinfo("Sending initialization data...\n");
                tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 80);
            }
        }

        centralProcedureInProgress = FALSE;                         // 清除操作进行中标志
    }
    else if(pMsg->method == ATT_HANDLE_VALUE_NOTI)                 // 如果是特征值通知
    {
        // 处理C2帧（版本号信息）
        if(pMsg->msg.handleValueNoti.pValue[0] == 0xC2)
        {
            // 解析C2帧中的版本号
            parseDeviceVersion(pMsg->msg.handleValueNoti.pValue, pMsg->msg.handleValueNoti.len);
        }
        // 处理C0帧（温度数据）
        else if(pMsg->msg.handleValueNoti.pValue[0] == 0xC0)
        {
            ulog_array_to_hex("Noti", pMsg->msg.handleValueNoti.pValue, pMsg->msg.handleValueNoti.len);
        }
        
        if(pMsg->msg.handleValueNoti.pValue[0]==0xc0)
        {
            uint8_t modetype = pMsg->msg.handleValueNoti.pValue[3];
            mode_type_golbal= pMsg->msg.handleValueNoti.pValue[3]+0x80;
            // 温度值需要转换为int8_t来正确处理负数（从机用uint8传输有符号温度）
            int8_t leftTemp = (int8_t)pMsg->msg.handleValueNoti.pValue[4];
            int8_t rightTemp = (int8_t)pMsg->msg.handleValueNoti.pValue[5];
            int8_t tempDelta = (int8_t)pMsg->msg.handleValueNoti.pValue[7];
            int8_t tempEnv = (int8_t)pMsg->msg.handleValueNoti.pValue[8];
            int8_t tempWater = (int8_t)pMsg->msg.handleValueNoti.pValue[9];
            uint8_t pwm_cold=pMsg->msg.handleValueNoti.pValue[11];
            uint8_t pwm_bump =pMsg->msg.handleValueNoti.pValue[13];
            uint8_t pwm_fan =pMsg->msg.handleValueNoti.pValue[14];
            uinfo("md=%d leftTemp=%d rightTemp=%d tempDelta=%d tempEnv=%d tempWater=%d pwm_cold=%d pwm_bump=%d pwm_fan=%d\n", 
                  modetype,leftTemp,rightTemp,tempDelta,tempEnv,tempWater,pwm_cold,pwm_bump,pwm_fan); 
            
            // 检测到错误状态（md=11），自动发送错误恢复命令
            if(modetype == 11)
            {
                uinfo("Error mode detected (md=11), sending recovery command D8 00 01 02...\n");
                
                // 组装20字节恢复命令
                uint8_t recovery_cmd[20] = {
                    0xD8, 0x00, 0x01, 0x02,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x00
                };
                
                // 检查是否可以发送（无其他GATT操作进行中）
                if(centralProcedureInProgress == FALSE && centralWriteCharHdl != 0)
                {
                    attWriteReq_t req;
                    req.cmd = TRUE;  // Write Command（无需响应）
                    req.sig = FALSE;
                    req.handle = centralWriteCharHdl;
                    req.len = 20;
                    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
                    
                    if(req.pValue != NULL)
                    {
                        tmos_memcpy(req.pValue, recovery_cmd, 20);
                        
                        bStatus_t status = GATT_WriteCharValue(centralConnHandle, &req, centralTaskId);
                        if(status == SUCCESS)
                        {
                            uinfo("Recovery command sent successfully!\n");
                        }
                        else
                        {
                            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
                            uinfo("Failed to send recovery command (status=0x%02X)\n", status);
                        }
                    }
                    else
                    {
                        uinfo("Failed to allocate memory for recovery command\n");
                    }
                }
                else
                {
                    uinfo("Cannot send recovery command (GATT busy or no write handle)\n");
                }
            }
            
#ifdef ENABLE_OLED_DISPLAY
            // 更新OLED显示 - 数据需要转换为0.1°C单位（原始数据是整数度）
            // int8转int16时会自动符号扩展，如-55 → -550
            // 连接状态：8表示已连接，版本号：使用解析到的版本号
            OLED_Update_Temp_Display(tempEnv * 10, leftTemp * 10, tempWater * 10, rightTemp * 10,
                                     tempDelta, modetype, pwm_cold,
                                     pwm_fan, pwm_bump, 8, deviceVersion);
#endif
        }
        
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
    // 不打印RSSI值，避免日志过多
    // uinfo("RSSI : -%d dB \n", -rssi);
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
    // 暂时禁用MTU交换，避免导致连接断开
    uinfo("MTU change detected: maxTx=%d, maxRx=%d (MTU exchange disabled)\n", maxTxOctets, maxRxOctets);
    
    // 直接触发服务发现，跳过MTU交换
    if(centralDiscState == BLE_DISC_STATE_SVC && centralSvcStartHdl == 0)
    {
        uinfo("Skipping MTU exchange, starting service discovery directly...\n");
        tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, 800); // 500ms后开始服务发现
    }
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
            uinfo("BLE \326\367\273\372\322\321\263\365\312\274\273\257,\325\375\324\332\313\321\313\367\311\350\261\270: %s / %s\n", 
                  TARGET_DEVICE_NAME_1, TARGET_DEVICE_NAME_2);  // 主机已初始化正在搜索设备
            
            // 更新OLED显示 - 阶段1：设备初始化完成
#ifdef ENABLE_OLED_DISPLAY
            OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 1, 0);
#endif
            
            // 初始化候选设备列表
            centralInitCandidates();
            
            // 只有在启用自动重连时才开始发现
            if(autoReconnectEnabled == TRUE)
            {
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,   // 开始设备发现
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
            }
        }
        break;

        case GAP_DEVICE_INFO_EVENT:                                // 设备信息事件
        {
            // 如果已经找到目标设备并正在连接，忽略新的广播包
            if(targetDeviceFound == TRUE || centralState != BLE_STATE_IDLE)
            {
                return;  // 直接返回，避免重复触发连接
            }
            
            // 先添加设备到列表（用于去重检查）
            centralAddDeviceInfo(pEvent->deviceInfo.addr, pEvent->deviceInfo.addrType);
            
            // 检查广播数据中的设备名称和RSSI
            uint8_t *pAdvData = pEvent->deviceInfo.pEvtData;
            uint8_t advDataLen = pEvent->deviceInfo.dataLen;
            int8_t rssi = pEvent->deviceInfo.rssi;  // 获取RSSI（如果可用）
            uint8_t i = 0;
            
            // 如果有广播数据，进行解析
            if(pAdvData != NULL && advDataLen > 0)
            {
                // 解析广播数据寻找设备名称
                while(i < advDataLen - 1)  // 确保至少有length和type字段
                {
                    uint8_t fieldLen = pAdvData[i];
                    if(fieldLen == 0 || i + fieldLen >= advDataLen) break;  // 防止越界
                    
                    uint8_t fieldType = pAdvData[i + 1];
                    
                    // 检查是否为完整本地名称(0x09)或缩短本地名称(0x08)
                    if(fieldType == 0x09 || fieldType == 0x08)
                    {
                        uint8_t nameLen = fieldLen - 1;  // 减去类型字段长度
                        if(nameLen > 0 && i + 2 + nameLen <= advDataLen)
                        {
                            // 提取设备名称
                            char devName[32] = {0};
                            uint8_t copyLen = (nameLen < 31) ? nameLen : 31;
                            for(uint8_t k = 0; k < copyLen; k++)
                            {
                                devName[k] = pAdvData[i + 2 + k];
                            }
                            
                            // 检查是否已经打印过此设备（避免重复日志）
                            uint8_t alreadyScanned = FALSE;
                            for(uint8_t s = 0; s < centralScanRes; s++)
                            {
                                if(tmos_memcmp(pEvent->deviceInfo.addr, centralDevList[s].addr, B_ADDR_LEN))
                                {
                                    alreadyScanned = TRUE;
                                    break;
                                }
                            }
                            
                            // 只有首次扫描到才打印日志
                            if(!alreadyScanned)
                            {
                                uinfo("[\311\250\303\350] %s (RSSI: %d dBm)\n", devName, rssi);  // 扫描
                            }
                            
                            // 检查是否匹配任一目标设备名称
                            uint8_t matchedNameIndex = 0;
                            
                            // 检查第一个目标名称 (HID-LongWang)
                            if(nameLen >= TARGET_DEVICE_NAME_1_LEN)
                            {
                                uint8_t match = 1;
                                for(uint8_t m = 0; m < TARGET_DEVICE_NAME_1_LEN; m++)
                                {
                                    if(pAdvData[i + 2 + m] != targetDeviceName1[m])
                                    {
                                        match = 0;
                                        break;
                                    }
                                }
                                if(match) matchedNameIndex = 1;
                            }
                            
                            // 检查第二个目标名称 (DragonK)
                            if(matchedNameIndex == 0 && nameLen >= TARGET_DEVICE_NAME_2_LEN)
                            {
                                uint8_t match = 1;
                                for(uint8_t m = 0; m < TARGET_DEVICE_NAME_2_LEN; m++)
                                {
                                    if(pAdvData[i + 2 + m] != targetDeviceName2[m])
                                    {
                                        match = 0;
                                        break;
                                    }
                                }
                                if(match) matchedNameIndex = 2;
                            }
                            
                            // 如果匹配到任一目标设备名称
                            if(matchedNameIndex > 0)
                            {
                                // 检查设备是否已在候选列表中
                                uint8_t alreadyAdded = FALSE;
                                for(uint8_t c = 0; c < MAX_CANDIDATES; c++)
                                {
                                    if(candidates[c].valid && 
                                       tmos_memcmp(pEvent->deviceInfo.addr, candidates[c].addr, B_ADDR_LEN))
                                    {
                                        alreadyAdded = TRUE;
                                        // 更新RSSI（如果更强）
                                        if(rssi > candidates[c].rssi)
                                        {
                                            candidates[c].rssi = rssi;
                                        }
                                        break;
                                    }
                                }
                                
                                // 只有新设备才打印和添加
                                if(!alreadyAdded)
                                {
                                    uinfo("[\272\362\321\241] %s (RSSI: %d dBm)\n", devName, rssi);  // 候选
                                    centralAddCandidate(pEvent->deviceInfo.addr, 
                                                       pEvent->deviceInfo.addrType,
                                                       rssi,
                                                       matchedNameIndex);
                                }
                            }
                            // if(matchedNameIndex > 0) 结束
                        }
                        // if(nameLen > 0 && ...) 结束
                    }
                    // if(fieldType == 0x09 || fieldType == 0x08) 结束
                    i += fieldLen + 1;  // 移动到下一个字段
                }
                // while 循环结束
            }
            // if(pAdvData != NULL && advDataLen > 0) 结束
        }
        break;

        case GAP_DEVICE_DISCOVERY_EVENT:                           // 设备发现事件
        {
            // 更新OLED显示 - 阶段2：设备发现完成
#ifdef ENABLE_OLED_DISPLAY
            OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 2, 0);
#endif
            
            // 扫描完成，从候选列表中选择信号最强的设备
            candidateDevice_t* bestCandidate = centralGetBestCandidate();
            
            if(bestCandidate != NULL)
            {
                // 找到最佳候选设备
                const char* devName = (bestCandidate->nameIndex == 1) ? TARGET_DEVICE_NAME_1 : TARGET_DEVICE_NAME_2;
                
                // 保存连接的设备名称
                tmos_memset(connectedDeviceName, 0, sizeof(connectedDeviceName));
                tmos_memcpy(connectedDeviceName, devName, tmos_strlen((char*)devName));
                
                uinfo("[\321\241\326\320] %s (RSSI: %d dBm)\n", connectedDeviceName, bestCandidate->rssi);  // 选中
                
                // 设置标志，防止重复触发连接
                targetDeviceFound = TRUE;
                
                // 如果已经有有效连接，先断开
                if(centralConnHandle != GAP_CONNHANDLE_INIT && centralState != BLE_STATE_IDLE)
                {
                    GAPRole_TerminateLink(centralConnHandle);
                    centralState = BLE_STATE_IDLE;
                    centralConnHandle = GAP_CONNHANDLE_INIT;
                    centralProcedureInProgress = FALSE;
                    DelayMs(300);
                }
                
                // 建立连接 - 使用高占空比扫描提高连接成功率
                bStatus_t status = GAPRole_CentralEstablishLink(TRUE,   // 使用高duty cycle扫描
                                                             FALSE,  // 不使用白名单
                                                             bestCandidate->addrType,
                                                             bestCandidate->addr);
                
                if(status == SUCCESS)
                {
                    centralState = BLE_STATE_CONNECTING;
                    connectionFailCount = 0;
                    tmos_start_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT, ESTABLISH_LINK_TIMEOUT * 2);
                    uinfo("\325\375\324\332\301\254\275\323 %s...\n", connectedDeviceName);  // 正在连接
                }
                else
                {
                    uinfo("\301\254\275\323\312\247\260\334 (0x%02X)\n", status);  // 连接失败
                    targetDeviceFound = FALSE;
                    connectionFailCount++;
                    
                    // 重试
                    if(autoReconnectEnabled == TRUE)
                    {
                        uint16_t retryDelay = (connectionFailCount >= 5) ? 4800 : 800;
                        if(connectionFailCount >= 5) connectionFailCount = 0;
                        tmos_start_task(centralTaskId, DELAYED_DISCOVERY_RETRY_EVT, retryDelay);
                    }
                }
            }
            else
            {
                // 没有找到候选设备
                centralScanRes = 0;
                
                // 重新开始搜索
                if(autoReconnectEnabled == TRUE)
                {
                    tmos_start_task(centralTaskId, DELAYED_DISCOVERY_RETRY_EVT, 800);  // 500ms后重试
                }
            }
        }
        break;

        case GAP_LINK_ESTABLISHED_EVENT:
        {
            tmos_stop_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT);
            
            if(pEvent->gap.hdr.status == SUCCESS)
            {
                // 更新OLED显示 - 阶段3：连接建立
#ifdef ENABLE_OLED_DISPLAY
                OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 3, 0);
#endif
                
                centralState = BLE_STATE_CONNECTED;
                centralConnHandle = pEvent->linkCmpl.connectionHandle;
                centralProcedureInProgress = FALSE;  // 重置GATT操作标志，允许服务发现
                svcDiscoveryRetryCount = 0;  // 重置服务发现重试计数器

                // 标记连接刚建立
                connectionJustEstablished = 1;

                uinfo("\322\321\301\254\275\323 %s\n", connectedDeviceName[0] ? (char*)connectedDeviceName : "Unknown");  // 已连接
                
                // 停止所有重连相关的定时事件
                tmos_stop_task(centralTaskId, DELAYED_DISCOVERY_RETRY_EVT);
                tmos_stop_task(centralTaskId, START_AUTO_RECONNECT_EVT);
                
                // 连接建立后，暂时跳过参数更新，直接开始服务发现
                uinfo("Connection established, starting service discovery directly...\n");
                tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, DEFAULT_SVC_DISCOVERY_DELAY);

                // 暂时跳过参数更新以专注于解决基本功能
                uinfo("Skipping parameter update to focus on basic functionality...\n");
            }
            else
            {
                centralScanRes = 0;
                targetDeviceFound = FALSE;  // 重置目标设备标志
                
                // 只有在启用自动重连时才重新开始搜索
                if(autoReconnectEnabled == TRUE)
                {
                    GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,
                                                  DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                                  DEFAULT_DISCOVERY_WHITE_LIST);
                }
            }
        }
        break;

        case GAP_LINK_TERMINATED_EVENT:
        {
            uint8_t reason = pEvent->linkTerminate.reason;

            uinfo("GAP_LINK_TERMINATED_EVENT: reason=0x%02X, connHandle=0x%04X\n",
                  reason, centralConnHandle);

            // 根据不同的断开原因进行诊断
            switch(reason)
            {
                case 0x08:  // CONNECTION_TIMEOUT_UNSPECIFIED
                    uinfo("[诊断] 连接超时 - 可能原因: 服务发现过早、连接参数不匹配\n");
                    break;
                case 0x13:  // REMOTE_USER_TERMINATED_CONN
                    uinfo("[诊断] 远程设备主动断开连接\n");
                    break;
                case 0x16:  // CONN_INTERVAL_UNACCEPTABLE
                    uinfo("[诊断] 连接参数不被接受 - 需要调整连接参数\n");
                    break;
                case 0x22:  // LMP_RESPONSE_TIMEOUT
                    uinfo("[诊断] LMP响应超时 - 可能是设备兼容性问题\n");
                    break;
                default:
                    uinfo("[诊断] 未知断开原因: 0x%02X\n", reason);
                    break;
            }

            centralState = BLE_STATE_IDLE;
            centralConnHandle = GAP_CONNHANDLE_INIT;
            centralDiscState = BLE_DISC_STATE_IDLE;
            centralNotifyCharHdl = 0;
            centralWriteCharHdl = 0;
            centralCharHdl = 0;
            centralCCCDHdl = 0;  // 重置CCCD句柄
            centralScanRes = 0;
            centralProcedureInProgress = FALSE;
            targetDeviceFound = FALSE;  // 重置目标设备找到标���
            connectionJustEstablished = 0;  // 重置连接刚建立标志

            
            // 停止所有相关的定时任务
            tmos_stop_task(centralTaskId, START_READ_RSSI_EVT);
            tmos_stop_task(centralTaskId, START_SVC_DISCOVERY_EVT);
            tmos_stop_task(centralTaskId, START_PARAM_UPDATE_EVT);
            tmos_stop_task(centralTaskId, START_AUTO_RECONNECT_EVT);
            tmos_stop_task(centralTaskId, DELAYED_DISCOVERY_RETRY_EVT);
            // tmos_stop_task(centralTaskId, START_HEARTBEAT_EVT);  // 心跳已禁用

            uinfo("\322\321\266\317\277\252\301\254\275\323\n");  // 已断开连接

#ifdef ENABLE_OLED_DISPLAY
            // 断开连接时显示"断"状态（mode_type=0xFF），温度全部为0避免歧义
            // 连接状态：0表示断开
            OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 0, 0);
#endif

            // 只有在启用自动重连时才重新搜索（根据错误原因调整延迟）
            if(autoReconnectEnabled == TRUE)
            {
                // 检查是否为用户主动触发的重连
                if(userTriggeredReconnect == TRUE)
                {
                    // 用户主动重连，立即开始扫描（不延迟）
                    userTriggeredReconnect = FALSE;  // 清除标志
                    tmos_set_event(centralTaskId, START_AUTO_RECONNECT_EVT);
                }
                else
                {
                    // 根据断开原因调整重连延迟
                    uint16_t reconnect_delay;

                    switch(reason)
                    {
                        case 0x08:  // 连接超时 - 增加延迟给从机更多准备时间
                            reconnect_delay = 1600;  // 1秒
                            uinfo("[策略] 连接超时，使用1秒延迟重连\n");
                            break;
                        case 0x16:  // 连接参数问题 - 使用较长延迟
                            reconnect_delay = 2400;  // 1.5秒
                            uinfo("[策略] 连接参数问题，使用1.5秒延迟重连\n");
                            break;
                        default:    // 其他原因 - 使用标准延迟
                            reconnect_delay = 800;   // 500ms
                            uinfo("[策略] 使用标准500ms延迟重连\n");
                            break;
                    }

                    bStatus_t status = tmos_start_task(centralTaskId, START_AUTO_RECONNECT_EVT, reconnect_delay);
                    if(status != SUCCESS)
                    {
                        // 如果失败，尝试立即触发（不延迟）
                        uinfo("[警告] 延迟重连启动失败，立即重连\n");
                        tmos_set_event(centralTaskId, START_AUTO_RECONNECT_EVT);
                    }
                }
            }
        }
        break;

        case GAP_LINK_PARAM_UPDATE_EVENT:
        {
            if(pEvent->linkUpdate.status == SUCCESS)
            {
                uinfo("[成功] 连接参数更新: 间隔=%d*1.25ms, 延迟=%d, 超时=%d*10ms\n",
                      pEvent->linkUpdate.connInterval,
                      pEvent->linkUpdate.connLatency,
                      pEvent->linkUpdate.connTimeout);

                // 验证参数是否在预期范围内
                uint16_t actualInterval = pEvent->linkUpdate.connInterval;
                if(actualInterval < DEFAULT_UPDATE_MIN_CONN_INTERVAL || actualInterval > DEFAULT_UPDATE_MAX_CONN_INTERVAL)
                {
                    uinfo("[警告] 从机返回的连接间隔超出请求范围: 请求[%d-%d], 实际[%d]\n",
                          DEFAULT_UPDATE_MIN_CONN_INTERVAL, DEFAULT_UPDATE_MAX_CONN_INTERVAL, actualInterval);
                }
            }
            else
            {
                uinfo("[失败] 连接参数更新失败: 0x%02X\n", pEvent->linkUpdate.status);

                // 根据错误代码提供具体诊断
                switch(pEvent->linkUpdate.status)
                {
                    case 0x01:  // 0x01 = LL_INVALID_PARAMS
                        uinfo("[诊断] 无效的连接参数 - 从机不支持请求的参数\n");
                        break;
                    case 0x02:  // 0x02 = LL_UNACCEPTABLE_CONN_INTERVAL
                        uinfo("[诊断] 连接间隔不可接受 - 需要使用更宽的参数范围\n");
                        break;
                    default:
                        uinfo("[诊断] 未知的参数更新错误: 0x%02X\n", pEvent->linkUpdate.status);
                        break;
                }

                // 即使参数更新失败，也继续进行服务发现
                uinfo("参数更新失败，但继续进行服务发现...\n");
                tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, DEFAULT_SVC_DISCOVERY_DELAY);
            }
        }
        break;

        case GAP_PHY_UPDATE_EVENT:
        {
            uinfo("PHY Update...\n");
        }
        break;

        
        case GAP_EXT_ADV_DEVICE_INFO_EVENT:
        {
            uinfo("Recv ext adv from %02X:%02X:%02X:%02X:%02X:%02X\n",
                  pEvent->deviceExtAdvInfo.addr[5], pEvent->deviceExtAdvInfo.addr[4],
                  pEvent->deviceExtAdvInfo.addr[3], pEvent->deviceExtAdvInfo.addr[2], 
                  pEvent->deviceExtAdvInfo.addr[1], pEvent->deviceExtAdvInfo.addr[0]);
            // Add device to list
            centralAddDeviceInfo(pEvent->deviceExtAdvInfo.addr, pEvent->deviceExtAdvInfo.addrType);
        }
        break;

        case GAP_DIRECT_DEVICE_INFO_EVENT:
        {
            uinfo("Recv direct adv from %02X:%02X:%02X:%02X:%02X:%02X\n",
                  pEvent->deviceDirectInfo.addr[5], pEvent->deviceDirectInfo.addr[4],
                  pEvent->deviceDirectInfo.addr[3], pEvent->deviceDirectInfo.addr[2], 
                  pEvent->deviceDirectInfo.addr[1], pEvent->deviceDirectInfo.addr[0]);
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
        uinfo("Pairing started:%d\n", status);                      // 打印配对开始
    }
    else if(state == GAPBOND_PAIRING_STATE_COMPLETE)               // 如果是配对完成状态
    {
        if(status == SUCCESS)                                       // 如果配对成功
        {
            uinfo("Pairing success\n");                             // 打印配对成功
        }
        else
        {
            uinfo("Pairing fail\n");                                // 打印配对失败
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BONDED)                 // 如果是已绑定状态
    {
        if(status == SUCCESS)                                       // 如果绑定成功
        {
            uinfo("Bonding success\n");                             // 打印绑定成功
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BOND_SAVED)             // 如果是绑定保存状态
    {
        if(status == SUCCESS)                                       // 如果保存成功
        {
            uinfo("Bond save success\n");                           // 打印绑定保存成功
        }
        else
        {
            uinfo("Bond save failed: %d\n", status);                // 打印绑定保存失败
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
        uinfo("Passcode:%06d\n", (int)passcode);                   // 打印6位密码
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
    // 检查连���状态和连接句柄是否有效
    if(centralState != BLE_STATE_CONNECTED)
    {
        uinfo("Service discovery aborted: not connected (state=%d)\n", centralState);
        return;
    }

    if(centralConnHandle == GAP_CONNHANDLE_INIT || centralConnHandle == 0xFFFE)
    {
        uinfo("Service discovery aborted: invalid connection handle (0x%04X)\n", centralConnHandle);
        return;
    }

    // 检查是否已经有GATT操作在进行中
    if(centralProcedureInProgress)
    {
        uinfo("Service discovery aborted: another GATT procedure is in progress\n");
        return;
    }

    // 检查连接稳定性 - 如果连接刚建立，需要延迟
    if(connectionJustEstablished)
    {
        uinfo("Service discovery delayed: connection just established, need stability time\n");
        // 连接刚建立，延迟一段时间再进行服务发现
        connectionJustEstablished = 0;  // 清除标志
        uinfo("Will retry service discovery after 500ms delay...\n");
        tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, 800);  // 500ms后重试
        return;
    }

    uinfo("Service discovery starting: connection should be stable now\n");
    
    // 更新OLED显示 - 阶段4：服务发现
#ifdef ENABLE_OLED_DISPLAY
    OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 4, 0);
#endif
    
    uint8_t uuid[ATT_BT_UUID_SIZE] = {LO_UINT16(TARGET_SERVICE_UUID),
                                      HI_UINT16(TARGET_SERVICE_UUID)}; // 目标服务UUID: AE00

    // Initialize cached handles                                    // 初始化缓存的句柄
    centralSvcStartHdl = centralSvcEndHdl = 0;
    centralNotifyCharHdl = centralWriteCharHdl = centralCharHdl = centralCCCDHdl = 0;

    centralDiscState = BLE_DISC_STATE_SVC;                         // 设置发现状态为服务发现

    // 设置GATT操作进行中标志
    centralProcedureInProgress = TRUE;

    // Discovery target BLE service (AE00)                        // 发现目标BLE服务(AE00)
    uinfo("Attempting service discovery: connHandle=0x%04X, taskId=%d, uuid=0x%04X\n",
          centralConnHandle, centralTaskId, TARGET_SERVICE_UUID);

    bStatus_t status = GATT_DiscPrimaryServiceByUUID(centralConnHandle,
                                                      uuid,
                                                      ATT_BT_UUID_SIZE,
                                                      centralTaskId);
    if(status != SUCCESS)
    {
        uinfo("Service discovery failed: 0x%02X (attempt %d/%d)\n", status, svcDiscoveryRetryCount + 1, MAX_SVC_DISCOVERY_RETRIES);

        // 重置GATT操作进行中标志
        centralProcedureInProgress = FALSE;

        // 检查是否需要重试
        if(svcDiscoveryRetryCount < MAX_SVC_DISCOVERY_RETRIES)
        {
            svcDiscoveryRetryCount++;
            uinfo("Retrying service discovery in %d ms...\n", SVC_DISCOVERY_RETRY_DELAY * 625 / 1000);
            tmos_start_task(centralTaskId, START_SVC_DISCOVERY_EVT, SVC_DISCOVERY_RETRY_DELAY);
        }
        else
        {
            uinfo("Service discovery failed after %d attempts, disconnecting...\n", MAX_SVC_DISCOVERY_RETRIES);
            svcDiscoveryRetryCount = 0; // 重置计数器
            // 断开连接
            GAPRole_TerminateLink(centralConnHandle);
            return; // 重要：直接返回，避免继续执行
        }
    }
    else
    {
        uinfo("Service discovery initiated successfully\n");
        svcDiscoveryRetryCount = 0; // 重置计数器
    }
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
        }
        // If procedure complete                                   // 如果程序完成
        if((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            if(pMsg->method == ATT_ERROR_RSP)
            {
                uinfo("Service discovery failed: 0x%02X\n", pMsg->msg.errorRsp.errCode);
            }
            
            if(centralSvcStartHdl != 0)                            // 如果找到AE00服务
            {
                // 更新OLED显示 - 阶段5：特征发现
#ifdef ENABLE_OLED_DISPLAY
                OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 5, 0);
#endif
                
                // Discover all characteristics in the service     // 发现服务中的所有特征
                centralDiscState = BLE_DISC_STATE_CHAR;            // 设置状态为特征发现
                GATT_DiscAllChars(centralConnHandle, centralSvcStartHdl, centralSvcEndHdl, centralTaskId);
            }
            else
            {
                uinfo("Target service AE00 not found!\n");        // AE00服务未找到
                // 断开连接，重新搜索设备
                GAPRole_TerminateLink(centralConnHandle);
            }
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CHAR)              // 如果是特征发现状态
    {
        // 处理特征发现响应
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            // 解析发现的所有特征
            for(uint8_t i = 0; i < pMsg->msg.readByTypeRsp.numPairs; i++)
            {
                uint8_t *pData = &pMsg->msg.readByTypeRsp.pDataList[i * pMsg->msg.readByTypeRsp.len];
                uint16_t valueHdl = BUILD_UINT16(pData[3], pData[4]);       // 特征值句柄
                
                // 对于16字节UUID，提取2字节短UUID
                if(pMsg->msg.readByTypeRsp.len == 21)  // 16字节UUID格式
                {
                    uint16_t shortUUID = BUILD_UINT16(pData[5], pData[6]);  // 提取短UUID
                    
                    // 检查是否为目标特征
                    if(shortUUID == TARGET_WRITE_CHAR_UUID)  // AE10写特征
                    {
                        centralWriteCharHdl = valueHdl;
                        centralCharHdl = valueHdl;  // 设置兼容变量
                    }
                    else if(shortUUID == TARGET_NOTIFY_CHAR_UUID)  // AE02通知特征
                    {
                        centralNotifyCharHdl = valueHdl;
                    }
                }
                else if(pMsg->msg.readByTypeRsp.len == 7)  // 2字节UUID格式
                {
                    uint16_t shortUUID = BUILD_UINT16(pData[5], pData[6]);
                    
                    if(shortUUID == TARGET_WRITE_CHAR_UUID)  // AE10写特征
                    {
                        centralWriteCharHdl = valueHdl;
                        centralCharHdl = valueHdl;  // 设置兼容变量
                    }
                    else if(shortUUID == TARGET_NOTIFY_CHAR_UUID)  // AE02通知特征
                    {
                        centralNotifyCharHdl = valueHdl;
                    }
                }
            }
        }
        
        if((pMsg->method == ATT_READ_BY_TYPE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            
            if(centralNotifyCharHdl != 0)  // 如果找到了AE02通知特征
            {
                // 更新OLED显示 - 阶段6：CCCD发现
#ifdef ENABLE_OLED_DISPLAY
                OLED_Update_Temp_Display(0, 0, 0, 0, 0, 0xFF, 0, 0, 0, 6, 0);
#endif
                
                // Discover CCCD for AE02 notification characteristic
                centralDiscState = BLE_DISC_STATE_CCCD;           // 设置状态为CCCD发现
                req.startHandle = centralSvcStartHdl;             // 设置起始句柄
                req.endHandle = centralSvcEndHdl;                 // 设置结束句柄
                req.type.len = ATT_BT_UUID_SIZE;                  // 设置UUID长度
                req.type.uuid[0] = LO_UINT16(GATT_CLIENT_CHAR_CFG_UUID); // 设置CCCD UUID
                req.type.uuid[1] = HI_UINT16(GATT_CLIENT_CHAR_CFG_UUID);

                // 不打印CCCD discovery详细信息
                GATT_ReadUsingCharUUID(centralConnHandle, &req, centralTaskId);
            }
            else
            {
                uinfo("===> [WARNING] AE02 notification characteristic not found!\n");
                uinfo("Notifications will not be available\n");
                centralDiscState = BLE_DISC_STATE_IDLE;
                centralProcedureInProgress = FALSE;
            }
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CCCD)             // 如果是CCCD发现状态
    {
        // CCCD found, store handle                              // 找到CCCD，存储句柄
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            centralCCCDHdl = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[0], // 构建CCCD句柄
                                          pMsg->msg.readByTypeRsp.pDataList[1]);
            centralProcedureInProgress = FALSE;                   // 清除操作进行中标志

            // RSSI监控暂时禁用 - 当前库版本可能不完全支持RSSI事件
            // uinfo("Starting RSSI monitoring for connection health check...\n");
            // tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD);

            // Start do write CCCD to enable notifications        // 开始写CCCD启用通知
            tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, DEFAULT_WRITE_CCCD_DELAY);

            // 不打印CCCD找到的详细信息
        }
        else
        {
            uinfo("===> [WARNING] AE02 CCCD not found, notifications not available\n");
            // 即使没有CCCD，连接仍然有效，可以进行写操作
            centralProcedureInProgress = FALSE;

            // RSSI监控暂时禁用 - 当前库版本可能不完全支持RSSI事件
            // uinfo("Starting RSSI monitoring for connection health check (no CCCD)...\n");
            // tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD);

            // 即使没有CCCD，也触发初始化数据发送
            if(centralWriteCharHdl != 0)
            {
                uinfo("Triggering initialization data send (no CCCD found)...\n");
                tmos_start_task(centralTaskId, START_SEND_INIT_DATA_EVT, 1000); // 1s后发送初始化数据
            }
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
//        uinfo("Device %d - Addr %x %x %x %x %x %x \n", centralScanRes,
//              centralDevList[centralScanRes - 1].addr[0],
//              centralDevList[centralScanRes - 1].addr[1],
//              centralDevList[centralScanRes - 1].addr[2],
//              centralDevList[centralScanRes - 1].addr[3],
//              centralDevList[centralScanRes - 1].addr[4],
//              centralDevList[centralScanRes - 1].addr[5]);
    }
}

/*********************************************************************
 * @fn      Central_DisconnectAndStopAutoReconnect
 *
 * @brief   断开当前连接并停止自动重连功能
 *
 * @return  none
 */
void Central_DisconnectAndStopAutoReconnect(void)
{
    uinfo("User triggered: Disconnect and stop auto reconnect\n");
    // 触发停止自动重连事件
    tmos_set_event(centralTaskId, STOP_AUTO_RECONNECT_EVT);
}

/*********************************************************************
 * @fn      Central_StartAutoReconnect
 *
 * @brief   启动自动搜索和连接功能
 *
 * @return  none
 */
void Central_StartAutoReconnect(void)
{
    uinfo("User triggered: Start auto reconnect\n");
    // 触发启动自动重连事件
    tmos_set_event(centralTaskId, START_AUTO_RECONNECT_EVT);
}

/*********************************************************************
 * @fn      Central_IsConnected
 *
 * @brief   检查是否当前有BLE连接
 *
 * @return  TRUE if connected, FALSE otherwise
 */
uint8_t Central_IsConnected(void)
{
    return (centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT);
}

/*********************************************************************
 * @fn      centralInitCandidates
 *
 * @brief   初始化候选设备列表
 *
 * @return  none
 */
static void centralInitCandidates(void)
{
    for(uint8_t i = 0; i < MAX_CANDIDATES; i++)
    {
        candidates[i].valid = FALSE;
        candidates[i].rssi = -128;  // 最弱信号
    }
    candidateCount = 0;
}

/*********************************************************************
 * @fn      centralAddCandidate
 *
 * @brief   添加候选设备到列表（智能优选，保留信号最强的设备）
 *
 * @param   addr - 设备地址
 * @param   addrType - 地址类型
 * @param   rssi - 信号强度
 * @param   nameIndex - 设备名称索引 (1 or 2)
 *
 * @return  none
 */
static void centralAddCandidate(uint8_t *addr, uint8_t addrType, int8_t rssi, uint8_t nameIndex)
{
    // 检查设备是否已经在列表中
    for(uint8_t i = 0; i < MAX_CANDIDATES; i++)
    {
        if(candidates[i].valid && tmos_memcmp(addr, candidates[i].addr, B_ADDR_LEN))
        {
            // 设备已存在，更新RSSI（如果更强）
            if(rssi > candidates[i].rssi)
            {
                candidates[i].rssi = rssi;
            }
            return;
        }
    }
    
    // 设备不在列表中，尝试添加
    if(candidateCount < MAX_CANDIDATES)
    {
        // 列表未满，直接添加
        for(uint8_t i = 0; i < MAX_CANDIDATES; i++)
        {
            if(!candidates[i].valid)
            {
                tmos_memcpy(candidates[i].addr, addr, B_ADDR_LEN);
                candidates[i].addrType = addrType;
                candidates[i].rssi = rssi;
                candidates[i].nameIndex = nameIndex;
                candidates[i].valid = TRUE;
                candidateCount++;
                return;
            }
        }
    }
    else
    {
        // 列表已满，找到信号最弱的设备
        uint8_t weakestIdx = 0;
        int8_t weakestRssi = candidates[0].rssi;
        
        for(uint8_t i = 1; i < MAX_CANDIDATES; i++)
        {
            if(candidates[i].rssi < weakestRssi)
            {
                weakestRssi = candidates[i].rssi;
                weakestIdx = i;
            }
        }
        
        // 如果新设备信号更强，替换最弱的
        if(rssi > weakestRssi)
        {
            tmos_memcpy(candidates[weakestIdx].addr, addr, B_ADDR_LEN);
            candidates[weakestIdx].addrType = addrType;
            candidates[weakestIdx].rssi = rssi;
            candidates[weakestIdx].nameIndex = nameIndex;
        }
    }
}

/*********************************************************************
 * @fn      centralGetBestCandidate
 *
 * @brief   获取信号最强的候选设备
 *
 * @return  指向最佳候选设备的指针，如果没有候选设备则返回NULL
 */
static candidateDevice_t* centralGetBestCandidate(void)
{
    if(candidateCount == 0)
    {
        return NULL;
    }
    
    uint8_t bestIdx = 0;
    int8_t bestRssi = -128;
    
    for(uint8_t i = 0; i < MAX_CANDIDATES; i++)
    {
        if(candidates[i].valid && candidates[i].rssi > bestRssi)
        {
            bestRssi = candidates[i].rssi;
            bestIdx = i;
        }
    }
    
    return &candidates[bestIdx];
}

/*********************************************************************
 * @fn      parseDeviceVersion
 *
 * @brief   Parse device version from C2 frame broadcast data.     // 从C2帧广播数据解析设备版本号
 *
 * @param   data - Pointer to broadcast data                       // 广播数据指针
 * @param   len - Length of broadcast data                         // 广播数据长度
 *
 * @return  none                                                   // 无返回值
 */
static void parseDeviceVersion(uint8_t *data, uint8_t len)
{
    // 检查是否是C2帧头：只检查第一个字节是否为0xC2
    if(len >= 9 && data[0] == 0xC2)
    {
        // 提取版本号：data7=0A(10), data8=45(69)
        uint8_t major_version = data[7];  // 主版本号
        uint8_t minor_version = data[8];  // 子版本号
        
        // 组合成16位版本号：高8位为主版本，低8位为子版本
        deviceVersion = (major_version << 8) | minor_version;
        
        uinfo("Device version parsed: %d.%d\n", major_version, minor_version);
    }
}

/************************ endfile @ central **************************/
