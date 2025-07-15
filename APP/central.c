/********************************** (C) COPYRIGHT *******************************
* File Name          : central.c                                                // �ļ�����central.c
* Author             : WCH                                                      // ���ߣ�WCH
* Version            : V1.1                                                     // �汾��V1.1
* Date               : 2020/08/06                                              // ���ڣ�2020/08/06
* Description        : �������̣�����ɨ����Χ�豸�������������Ĵӻ��豸��ַ��           // ����������ʾ������
*                      Ѱ���Զ������������ִ�ж�д�������ӻ��������ʹ��,        //      ɨ�貢����ָ���ӻ�
                       �����ӻ��豸��ַ�޸�Ϊ������Ŀ���ַ��Ĭ��Ϊ(84:C2:E4:03:02:02)  //      Ĭ��Ŀ���ַ
*******************************************************************************/

/*********************************************************************
 * INCLUDES                                                           // ����ͷ�ļ�
 */
#include "CONFIG.h"                                                   // ��������ͷ�ļ�
#include "gattprofile.h"                                             // ����GATT�����ļ�ͷ�ļ�
#include "central.h"                                                 // ����central����ͷ�ļ�
#include "key.h"   
/*********************************************************************
 * MACROS                                                            // �궨��
 */

// Length of bd addr as a string                                     
#define B_ADDR_STR_LEN                      15                       // ������ַ�ַ������ȶ���Ϊ15

/*********************************************************************
 * CONSTANTS                                                         // ��������
 */
// Maximum number of scan responses                                  
#define DEFAULT_MAX_SCAN_RES                50                       // ���ɨ����Ӧ����Ϊ50

// Scan duration in 0.625ms                                         
#define DEFAULT_SCAN_DURATION               2400                     // ɨ�����ʱ�䣬��λ0.625ms

// Connection min interval in 1.25ms                                
#define DEFAULT_MIN_CONNECTION_INTERVAL     20                       // ��С���Ӽ������λ1.25ms

// Connection max interval in 1.25ms                                
#define DEFAULT_MAX_CONNECTION_INTERVAL     100                      // ������Ӽ������λ1.25ms

// Connection supervision timeout in 10ms                           
#define DEFAULT_CONNECTION_TIMEOUT          100                      // ���Ӽල��ʱ����λ10ms

// Discovey mode (limited, general, all)                            
#define DEFAULT_DISCOVERY_MODE              DEVDISC_MODE_ALL         // ����ģʽ����Ϊȫ������

// TRUE to use active scan                                          
#define DEFAULT_DISCOVERY_ACTIVE_SCAN       TRUE                     // ʹ������ɨ��

// TRUE to use white list during discovery                          
#define DEFAULT_DISCOVERY_WHITE_LIST        FALSE                    // ��ʹ�ð��������з���

// TRUE to use high scan duty cycle when creating link              
#define DEFAULT_LINK_HIGH_DUTY_CYCLE        FALSE                   // ��������ʱ��ʹ�ø�ռ�ձ�ɨ��

// TRUE to use white list when creating link                        
#define DEFAULT_LINK_WHITE_LIST             FALSE                   // ��������ʱ��ʹ�ð�����

// Default read RSSI period in 0.625ms                             
#define DEFAULT_RSSI_PERIOD                 2400                    // Ĭ��RSSI��ȡ���ڣ���λ0.625ms

// Minimum connection interval (units of 1.25ms)                    
#define DEFAULT_UPDATE_MIN_CONN_INTERVAL    20                      // �������Ӳ�������С���

// Maximum connection interval (units of 1.25ms)                    
#define DEFAULT_UPDATE_MAX_CONN_INTERVAL    100                     // �������Ӳ����������

// Slave latency to use parameter update                            
#define DEFAULT_UPDATE_SLAVE_LATENCY        0                       // �ӻ��ӳٲ���

// Supervision timeout value (units of 10ms)                        
#define DEFAULT_UPDATE_CONN_TIMEOUT         600                     // ���ӳ�ʱֵ����λ10ms

// Default passcode                                                 
#define DEFAULT_PASSCODE                    0                       // Ĭ���������

// Default GAP pairing mode                                         
#define DEFAULT_PAIRING_MODE                GAPBOND_PAIRING_MODE_INITIATE // Ĭ��GAP���ģʽΪ�������

// Default MITM mode (TRUE to require passcode or OOB when pairing) 
#define DEFAULT_MITM_MODE                   TRUE                    // Ĭ������MITM����

// Default bonding mode, TRUE to bond, max bonding 6 devices        
#define DEFAULT_BONDING_MODE                TRUE                    // Ĭ�����ð�ģʽ������6���豸

// Default GAP bonding I/O capabilities                             
#define DEFAULT_IO_CAPABILITIES             GAPBOND_IO_CAP_DISPLAY_ONLY // Ĭ��GAP��I/O����Ϊ����ʾ

// Default service discovery timer delay in 0.625ms                 
#define DEFAULT_SVC_DISCOVERY_DELAY         1600                    // Ĭ�Ϸ�������ʱ

// Default parameter update delay in 0.625ms                        
#define DEFAULT_PARAM_UPDATE_DELAY          3200                    // Ĭ�ϲ���������ʱ

// Default phy update delay in 0.625ms                              
#define DEFAULT_PHY_UPDATE_DELAY            2400                    // Ĭ��PHY������ʱ

// Default read or write timer delay in 0.625ms                     
#define DEFAULT_READ_OR_WRITE_DELAY         1600                    // Ĭ�϶�д������ʱ

// Default write CCCD delay in 0.625ms                              
#define DEFAULT_WRITE_CCCD_DELAY            1600                    // Ĭ��дCCCD��ʱ

// Establish link timeout in 0.625ms                                
#define ESTABLISH_LINK_TIMEOUT              3200                    // �������ӳ�ʱʱ��

// Application states                                               // Ӧ��״̬ö��
enum
{
    BLE_STATE_IDLE,                                                // ����״̬
    BLE_STATE_CONNECTING,                                          // ��������״̬
    BLE_STATE_CONNECTED,                                          // ������״̬
    BLE_STATE_DISCONNECTING                                       // ���ڶϿ�����״̬
};

// Discovery states                                                   // ����״̬����
#define BLE_DISC_STATE_IDLE               0x00                     // ����״̬
#define BLE_DISC_STATE_SVC                0x01                     // AE00������״̬
#define BLE_DISC_STATE_CHAR               0x02                     // AE00��������״̬

/*********************************************************************
 * TYPEDEFS                                                          // ���Ͷ���
 */

/*********************************************************************
 * GLOBAL VARIABLES                                                  // ȫ�ֱ���
 */

/*********************************************************************
 * EXTERNAL VARIABLES                                                // �ⲿ����
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS                                                // �ⲿ����
 */

/*********************************************************************
 * LOCAL VARIABLES                                                   // ���ر���
 */

// Task ID for internal task/event processing                        
 uint8_t centralTaskId;                                        // �����豸����ID

// Number of scan results                                            
static uint8_t centralScanRes;                                       // ɨ��������

// Scan result list                                                  
static gapDevRec_t centralDevList[DEFAULT_MAX_SCAN_RES];            // ɨ�����б�
static extDeviceInfo_t centralExtDevList[DEFAULT_MAX_SCAN_RES];     // ��չ�豸��Ϣ�б�

// Peer device address (deprecated, now using name/VID/PID matching)                                               
static uint8_t PeerAddrDef[B_ADDR_LEN] = {0x03, 0x03, 0x03, 0xE4, 0xC2, 0x84}; // �Զ��豸��ַ

// Current target device index
static uint8_t targetDeviceIndex = 0xFF;

// RSSI polling state                                                
static uint8_t centralRssi = TRUE;                                   // RSSI��ѯ״̬

// Parameter update state                                            
static uint8_t centralParamUpdate = TRUE;                           // ��������״̬

// Phy update state                                                  
static uint8 centralPhyUpdate = FALSE;                              // PHY����״̬

// Connection handle of current connection                           
static uint16_t centralConnHandle = GAP_CONNHANDLE_INIT;            // ��ǰ���Ӿ��

// Application state                                                 
static uint8_t centralState = BLE_STATE_IDLE;                       // Ӧ��״̬

// Discovery state                                                   
static uint8_t centralDiscState = BLE_DISC_STATE_IDLE;              // ����״̬

// AE00 Service specific handles (BLE channel for data transmission)
static uint16_t centralAE00SvcStartHdl = 0;                         // AE00������ʼ���
static uint16_t centralAE00SvcEndHdl = 0;                           // AE00����������
static uint16_t centralAE10CharHdl = 0;                             // AE10�������
static uint16_t centralCCCDHdl = 0;                                 // CCCD���

// GATT read/write procedure state                                   
static uint8_t centralProcedureInProgress = FALSE;                  // GATT��/д����״̬
static uint8_t centralDoWrite = FALSE;                              // ��д������־
static uint8_t centralCharVal = 0x01;                               // д��������ֵ

/*********************************************************************
 * LOCAL FUNCTIONS                                                   // ���غ�������
 */
static void centralProcessGATTMsg(gattMsgEvent_t *pMsg);            // ����GATT��Ϣ
static void centralRssiCB(uint16_t connHandle, int8_t rssi);        // RSSI�ص�����
static void centralEventCB(gapRoleEvent_t *pEvent);                 // �¼��ص�����
static void centralHciMTUChangeCB(uint16_t connHandle, uint16_t maxTxOctets, uint16_t maxRxOctets); // MTU����ص�
static void centralPasscodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                              uint8_t uiInputs, uint8_t uiOutputs);  // ����ص�����
static void centralPairStateCB(uint16_t connHandle, uint8_t state, uint8_t status); // ���״̬�ص�
static void central_ProcessTMOSMsg(tmos_event_hdr_t *pMsg);         // ����TMOS��Ϣ
static void centralGATTDiscoveryEvent(gattMsgEvent_t *pMsg);        // GATT�����¼�����
static void centralStartDiscovery(void);                            // ��ʼ������
static void centralAddDeviceInfo(uint8_t *pAddr, uint8_t addrType); // ����豸��Ϣ
static uint8_t centralParseAdvertData(uint8_t *pData, uint8_t dataLen, uint8_t *deviceName, uint8_t *nameLen); // �����㲥����
static uint8_t centralFindTargetDevice(void); // ����Ŀ���豸
static void centralReadDeviceInfo(void); // ��ȡ�豸��Ϣ

/*********************************************************************
 * PROFILE CALLBACKS                                                 // �����ļ��ص�
 */

// GAP Role Callbacks                                                // GAP��ɫ�ص��ṹ��
static gapCentralRoleCB_t centralRoleCB = {
    centralRssiCB,        // RSSI callback                          // RSSI�ص�
    centralEventCB,       // Event callback                         // �¼��ص�
    centralHciMTUChangeCB // MTU change callback                    // MTU����ص�
};

// Bond Manager Callbacks                                            // �󶨹������ص��ṹ��
static gapBondCBs_t centralBondCB = {
    centralPasscodeCB,    // Passcode callback                      // ����ص�
    centralPairStateCB    // Pairing state callback                 // ���״̬�ص�
};

/*********************************************************************
 * PUBLIC FUNCTIONS                                                  // ��������
 */

/*********************************************************************
 * @fn      Central_Init
 *
 * @brief   Initialization function for the Central App Task.        // �����豸Ӧ������ĳ�ʼ������
 *          This is called during initialization and should contain  // ��������ڳ�ʼ��ʱ�����ã�Ӧ�ð���
 *          any application specific initialization (ie. hardware    // �κ�Ӧ�ó����ض��ĳ�ʼ������Ӳ��
 *          initialization/setup, table initialization, power up     // ��ʼ��/���ã����ʼ�����ϵ�
 *          notification).                                          // ֪ͨ��
 *
 * @param   task_id - the ID assigned by TMOS.  This ID should be   // task_id - TMOS�����ID�����IDӦ��
 *                    used to send messages and set timers.         // ���ڷ�����Ϣ�����ö�ʱ��
 *
 * @return  none                                                    // �޷���ֵ
 */
void Central_Init()
{
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);  // ע�������豸�¼�������
    
    // Setup GAP                                                      // ����GAP����
    GAP_SetParamValue(TGAP_DISC_SCAN, DEFAULT_SCAN_DURATION);        // ����ɨ�����ʱ��
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, DEFAULT_MIN_CONNECTION_INTERVAL); // ������С���Ӽ��
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, DEFAULT_MAX_CONNECTION_INTERVAL); // ����������Ӽ��
    GAP_SetParamValue(TGAP_CONN_EST_SUPERV_TIMEOUT, DEFAULT_CONNECTION_TIMEOUT); // �������ӳ�ʱʱ��

    // Setup the GAP Bond Manager                                     // ����GAP�󶨹�����
    {
        uint32_t passkey = DEFAULT_PASSCODE;                          // ����Ĭ������
        uint8_t  pairMode = DEFAULT_PAIRING_MODE;                    // �������ģʽ
        uint8_t  mitm = DEFAULT_MITM_MODE;                           // ����MITM����ģʽ
        uint8_t  ioCap = DEFAULT_IO_CAPABILITIES;                    // ����IO����
        uint8_t  bonding = DEFAULT_BONDING_MODE;                     // ���ð�ģʽ

        GAPBondMgr_SetParameter(GAPBOND_CENT_DEFAULT_PASSCODE, sizeof(uint32_t), &passkey);  // ����Ĭ������
        GAPBondMgr_SetParameter(GAPBOND_CENT_PAIRING_MODE, sizeof(uint8_t), &pairMode);      // �������ģʽ
        GAPBondMgr_SetParameter(GAPBOND_CENT_MITM_PROTECTION, sizeof(uint8_t), &mitm);       // ����MITM����
        GAPBondMgr_SetParameter(GAPBOND_CENT_IO_CAPABILITIES, sizeof(uint8_t), &ioCap);      // ����IO����
        GAPBondMgr_SetParameter(GAPBOND_CENT_BONDING_ENABLED, sizeof(uint8_t), &bonding);    // ���ð�ʹ��
    }

    // Initialize GATT Client                                         // ��ʼ��GATT�ͻ���
    GATT_InitClient();
    // Register to receive incoming ATT Indications/Notifications     // ע�����ATTָʾ/֪ͨ
    GATT_RegisterForInd(centralTaskId);
    // Setup a delayed profile startup                               // �����ӳٵ������ļ�����
    tmos_set_event(centralTaskId, START_DEVICE_EVT);
}

/*********************************************************************
 * @fn      Central_ProcessEvent
 *
 * @brief   Central Application Task event processor.                // ����Ӧ�������¼�������
 *          This function is called to process all events for the task. // �����������������������������¼�
 *          Events include timers, messages and any other user defined  // �¼�������ʱ������Ϣ���κ������û�������¼�
 *          events.
 *
 * @param   task_id  - The TMOS assigned task ID.                   // task_id - TMOS���������ID
 * @param   events - events to process.  This is a bit map and can  // events - Ҫ������¼�������һ��λͼ
 *                   contain more than one event.                   // ���԰�������¼�
 *
 * @return  events not processed                                    // ����δ������¼�
 */
uint16_t Central_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & SYS_EVENT_MSG)                                       // �����ϵͳ��Ϣ�¼�
    {
        uint8_t *pMsg;

        if((pMsg = tmos_msg_receive(centralTaskId)) != NULL)         // ������Ϣ
        {
            central_ProcessTMOSMsg((tmos_event_hdr_t *)pMsg);        // ����TMOS��Ϣ
            // Release the TMOS message                               // �ͷ�TMOS��Ϣ
            tmos_msg_deallocate(pMsg);
        }
        // return unprocessed events                                 // ����δ������¼�
        return (events ^ SYS_EVENT_MSG);
    }

    if(events & START_DEVICE_EVT)                                    // ����������豸�¼�
    {
        // Start the Device                                          // �����豸
        GAPRole_CentralStartDevice(centralTaskId, &centralBondCB, &centralRoleCB);
        return (events ^ START_DEVICE_EVT);
    }

    if(events & ESTABLISH_LINK_TIMEOUT_EVT)                              // ����ǽ������ӳ�ʱ�¼�
    {
        GAPRole_TerminateLink(INVALID_CONNHANDLE);                    // ��ֹ����
        return (events ^ ESTABLISH_LINK_TIMEOUT_EVT);
    }

    if(events & START_SVC_DISCOVERY_EVT)                              // ����ǿ�ʼ�������¼�
    {
        // start service discovery                                     // ��ʼ������
        centralStartDiscovery();
        return (events ^ START_SVC_DISCOVERY_EVT);
    }

    if(events & START_PARAM_UPDATE_EVT)                               // ����ǿ�ʼ���������¼�
    {
        // start connect parameter update                              // ��ʼ���Ӳ�������
        GAPRole_UpdateLink(centralConnHandle,
                           DEFAULT_UPDATE_MIN_CONN_INTERVAL,           // ʹ��Ĭ�ϵ���С���Ӽ��
                           DEFAULT_UPDATE_MAX_CONN_INTERVAL,           // ʹ��Ĭ�ϵ�������Ӽ��
                           DEFAULT_UPDATE_SLAVE_LATENCY,               // ʹ��Ĭ�ϵĴӻ��ӳ�
                           DEFAULT_UPDATE_CONN_TIMEOUT);               // ʹ��Ĭ�ϵ����ӳ�ʱ
        return (events ^ START_PARAM_UPDATE_EVT);
    }

    if(events & START_PHY_UPDATE_EVT)                                 // ����ǿ�ʼPHY�����¼�
    {
        // start phy update                                           // ��ʼPHY����
        PRINT("PHY Update %x...\n", GAPRole_UpdatePHY(centralConnHandle, 0, 
                    GAP_PHY_BIT_LE_2M, GAP_PHY_BIT_LE_2M, GAP_PHY_OPTIONS_NOPRE));

        return (events ^ START_PHY_UPDATE_EVT);
    }

    if(events & START_READ_OR_WRITE_EVT)                              // ����ǿ�ʼ��д�¼�
    {
        if(centralProcedureInProgress == FALSE)                       // �����ǰû�����ڽ��еĲ���
        {
            if(centralDoWrite)                                        // �����д����
            {
                // �򻯵�д����
                attWriteReq_t req;

                req.cmd = FALSE;                                      // ��������
                req.sig = FALSE;                                      // ����ǩ��
                req.handle = centralAE10CharHdl;                      // ʹ�÷��ֵ��������
                req.len = 1;                                          // д�볤��Ϊ1
                req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // �����ڴ�
                if(req.pValue != NULL)                                // ����ڴ����ɹ�
                {
                    req.pValue[0] = centralCharVal;                   // д��ֵ

                    if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // д������ֵ
                    {
                        centralProcedureInProgress = TRUE;            // ���ò��������б�־
                        centralDoWrite = !centralDoWrite;             // �л���д��־
                    }
                    else
                    {
                        GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ); // �ͷ��ڴ�
                    }
                }
            }
            else                                                     // ����ִ�ж�����
            {
                // Do a read                                         // ִ�ж�����
                attReadReq_t req;

                req.handle = centralAE10CharHdl;                     // �����������
                if(GATT_ReadCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // ��ȡ����ֵ
                {
                    centralProcedureInProgress = TRUE;               // ���ò��������б�־
                    centralDoWrite = !centralDoWrite;                // �л���д��־
                }
            }
        }

        return (events ^ START_READ_OR_WRITE_EVT);
    }

    if(events & START_WRITE_CCCD_EVT)                                   // ����ǿ�ʼдCCCD�¼�
    {
        if(centralProcedureInProgress == FALSE)                        // �����ǰû�����ڽ��еĲ���
        {
            // Do a write                                              // ִ��д����
            attWriteReq_t req;

            req.cmd = FALSE;                                           // ��������
            req.sig = FALSE;                                           // ����ǩ��
            req.handle = centralCCCDHdl;                               // ����CCCD���
            req.len = 2;                                               // д�볤��Ϊ2
            req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // �����ڴ�
            printf("CCCD_handle = %x\n" ,req.handle);                  // ��ӡ���ֵ
            if(req.pValue != NULL)                                     // ����ڴ����ɹ�
            {
                req.pValue[0] = 1;                                     // ����֪ͨ
                req.pValue[1] = 0;

                if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // д��CCCDֵ
                {
                    centralProcedureInProgress = TRUE;                 // ���ò��������б�־
                }
                else
                {
                    GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);   // �ͷ��ڴ�
                }
            }
        }
        return (events ^ START_WRITE_CCCD_EVT);
    }

    if(events & START_READ_RSSI_EVT)                                  // ����ǿ�ʼ��ȡRSSI�¼�
    {
        GAPRole_ReadRssiCmd(centralConnHandle);                       // ��ȡRSSIֵ
        tmos_start_task(centralTaskId, START_READ_RSSI_EVT, DEFAULT_RSSI_PERIOD); // ������һ��RSSI��ȡ
        return (events ^ START_READ_RSSI_EVT);
    }

    if(events & START_HID_TEST_DATA_EVT)                             // �����HID�������ݷ����¼�
    {
        SendHIDTestData();                                           // ����HID��������
        return (events ^ START_HID_TEST_DATA_EVT);
    }

    if(events & START_READ_DEVICE_INFO_EVT)                          // ����Ƕ�ȡ�豸��Ϣ�¼�
    {
        centralReadDeviceInfo();                                     // ��ȡ�豸��Ϣ
        return (events ^ START_READ_DEVICE_INFO_EVT);
    }

    // Discard unknown events                                         // ����δ֪�¼�
    return 0;
}

/*********************************************************************
 * @fn      central_ProcessTMOSMsg
 *
 * @brief   Process an incoming task message.                         // �������������Ϣ
 *
 * @param   pMsg - message to process                                // pMsg - Ҫ�������Ϣ
 *
 * @return  none                                                     // �޷���ֵ
 */
static void central_ProcessTMOSMsg(tmos_event_hdr_t *pMsg)
{
    switch(pMsg->event)                                               // ������Ϣ�¼����ʹ���
    {
        case GATT_MSG_EVENT:                                          // �����GATT��Ϣ�¼�
            centralProcessGATTMsg((gattMsgEvent_t *)pMsg);            // ����GATT��Ϣ
            break;
    }
}

/*********************************************************************
 * @fn      centralProcessGATTMsg
 *
 * @brief   Process GATT messages                                    // ����GATT��Ϣ
 *
 * @return  none                                                     // �޷���ֵ
 */
static void centralProcessGATTMsg(gattMsgEvent_t *pMsg)
{
    uint16 i = 0;                                                     // ѭ��������
    if(centralState != BLE_STATE_CONNECTED)                           // �������������״̬
    {
        // In case a GATT message came after a connection has dropped,  // ��������ѶϿ����յ�GATT��Ϣ
        // ignore the message                                           // ���Ը���Ϣ
        GATT_bm_free(&pMsg->msg, pMsg->method);
        return;
    }

    if((pMsg->method == ATT_EXCHANGE_MTU_RSP) ||                        // �����MTU������Ӧ
       ((pMsg->method == ATT_ERROR_RSP) &&                            // �����Ǵ�����Ӧ
        (pMsg->msg.errorRsp.reqOpcode == ATT_EXCHANGE_MTU_REQ)))      // ����MTU��������Ĵ���
    {
        if(pMsg->method == ATT_ERROR_RSP)                             // ����Ǵ�����Ӧ
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;              // ��ȡ������

            PRINT("Exchange MTU Error: %x\n", status);                 // ��ӡMTU��������
        }
        centralProcedureInProgress = FALSE;                           // ������������б�־
    }

    if(pMsg->method == ATT_MTU_UPDATED_EVENT)                         // �����MTU�����¼�
    {
        PRINT("MTU: %x\n", pMsg->msg.mtuEvt.MTU);                    // ��ӡ�µ�MTUֵ
    }

    if((pMsg->method == ATT_READ_RSP) ||                             // ����Ƕ�ȡ��Ӧ
       ((pMsg->method == ATT_ERROR_RSP) &&                           // �����Ǵ�����Ӧ
        (pMsg->msg.errorRsp.reqOpcode == ATT_READ_REQ)))             // ���Ƕ�ȡ����Ĵ���
    {
        if(pMsg->method == ATT_ERROR_RSP)                            // ����Ǵ�����Ӧ
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;             // ��ȡ������

            PRINT("Read Error: %x\n", status);                        // ��ӡ��ȡ����
        }
        else
        {
            // After a successful read, display the read value         // �ɹ���ȡ����ʾ��ȡ��ֵ
//            PRINT("Read rsp: %x\n", *pMsg->msg.readRsp.pValue);
//          PRINT("len = %d, Read rsp: ", pMsg->msg.readRsp.len);
//          for( i = 0; i < pMsg->msg.readRsp.len; i++){
//            PRINT("%02x ", pMsg->msg.readRsp.pValue[i]);
//          }PRINT("\n");
        }
//        tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, DEFAULT_WRITE_CCCD_DELAY);
//        tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY);
        centralProcedureInProgress = FALSE;                          // ������������б�־
    }
    else if((pMsg->method == ATT_WRITE_RSP) ||                      // �����д����Ӧ
            ((pMsg->method == ATT_ERROR_RSP) &&                      // �����Ǵ�����Ӧ
             (pMsg->msg.errorRsp.reqOpcode == ATT_WRITE_REQ)))      // ����д������Ĵ���
    {
        if(pMsg->method == ATT_ERROR_RSP)                           // ����Ǵ�����Ӧ
        {
            uint8_t status = pMsg->msg.errorRsp.errCode;            // ��ȡ������

            PRINT("HID Write Error: 0x%02X ", status);               // ��ӡд�����
            switch(status) {
                case 0x03: PRINT("(Write Not Permitted - д�벻������)\n"); break;
                case 0x08: PRINT("(Insufficient Authentication - ��֤����)\n"); break;
                case 0x05: PRINT("(Insufficient Authorization - ��Ȩ����)\n"); break;
                case 0x0F: PRINT("(Insufficient Key Size - ��Կ���Ȳ���)\n"); break;
                default: PRINT("(Unknown Error - δ֪����)\n"); break;
            }
        }
        else
        {
            // Write success                                         // д��ɹ�
            PRINT("HID Write success - �����ѳɹ����͵�HID�豸\n");     // ��ӡд��ɹ�
        }

        centralProcedureInProgress = FALSE;                         // ������������б�־
    }
    else if(pMsg->method == ATT_HANDLE_VALUE_NOTI)                 // ���������ֵ֪ͨ
    {
//        PRINT("Receive noti: %x\n", *pMsg->msg.handleValueNoti.pValue);
      PRINT("Noti: ");                                             // ��ӡֵ֪ͨ
      for( i = 0; i < pMsg->msg.handleValueNoti.len; i++){         // ѭ����ӡÿ���ֽ�
        PRINT("%02x ", pMsg->msg.handleValueNoti.pValue[i]);       // ��16���Ƹ�ʽ��ӡ
      }PRINT("\n");

    }
    else if(centralDiscState != BLE_DISC_STATE_IDLE)               // ������Ƿ��ֿ���״̬
    {
        centralGATTDiscoveryEvent(pMsg);                           // ����GATT�����¼�
    }
    GATT_bm_free(&pMsg->msg, pMsg->method);                       // �ͷ���Ϣ�ڴ�
}

/*********************************************************************
 * @fn      centralRssiCB
 *
 * @brief   RSSI callback.                                          // RSSI�ص�����
 *
 * @param   connHandle - connection handle                          // connHandle - ���Ӿ��
 * @param   rssi - RSSI                                            // rssi - RSSIֵ
 *
 * @return  none                                                    // �޷���ֵ
 */
static void centralRssiCB(uint16_t connHandle, int8_t rssi)
{
    PRINT("RSSI : -%d dB \n", -rssi);                              // ��ӡRSSIֵ
}

/*********************************************************************
 * @fn      centralHciMTUChangeCB
 *
 * @brief   MTU changed callback.                                   // MTU����ص�����
 *
 * @param   maxTxOctets - Max tx octets                            // maxTxOctets - ������ֽ���
 * @param   maxRxOctets - Max rx octets                            // maxRxOctets - �������ֽ���
 *
 * @return  none                                                    // �޷���ֵ
 */
static void centralHciMTUChangeCB(uint16_t connHandle, uint16_t maxTxOctets, uint16_t maxRxOctets)
{
    attExchangeMTUReq_t req;                                        // MTU��������ṹ��

    req.clientRxMTU = maxRxOctets;                                 // ���ÿͻ��˽���MTU
    GATT_ExchangeMTU(connHandle, &req, centralTaskId);             // ����MTU��������
    PRINT("exchange mtu:%d\n", maxRxOctets);                       // ��ӡ������MTUֵ
    centralProcedureInProgress = TRUE;                             // ���ò��������б�־
}

/*********************************************************************
 * @fn      centralEventCB
 *
 * @brief   Central event callback function.                        // �����豸�¼��ص�����
 *
 * @param   pEvent - pointer to event structure                     // pEvent - �¼��ṹ��ָ��
 *
 * @return  none                                                    // �޷���ֵ
 */
static void centralEventCB(gapRoleEvent_t *pEvent)
{
    switch(pEvent->gap.opcode)                                      // �����¼������봦��
    {
        case GAP_DEVICE_INIT_DONE_EVENT:                           // �豸��ʼ������¼�
        {
            PRINT("Discovering...\n");                              // ��ӡ��ʼ�����豸
            GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE,   // ��ʼ�豸����
                                          DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                          DEFAULT_DISCOVERY_WHITE_LIST);
        }
        break;

        case GAP_DEVICE_INFO_EVENT:                                // �豸��Ϣ�¼�
        {
            // Parse advertising data to extract device name        // �����㲥������ȡ�豸��
            uint8_t deviceName[32] = {0};
            uint8_t nameLen = 0;
            
            centralParseAdvertData(pEvent->deviceInfo.pEvtData, 
                                 pEvent->deviceInfo.dataLen, 
                                 deviceName, &nameLen);
            
            // Add device to list with parsed information           // ����豸���б�
            centralAddDeviceInfo(pEvent->deviceInfo.addr, pEvent->deviceInfo.addrType);
            
            // Store extended device information                    // �洢��չ�豸��Ϣ
            if(centralScanRes > 0)
            {
                uint8_t idx = centralScanRes - 1;
                tmos_memcpy(centralExtDevList[idx].addr, pEvent->deviceInfo.addr, B_ADDR_LEN);
                centralExtDevList[idx].addrType = pEvent->deviceInfo.addrType;
                tmos_memcpy(centralExtDevList[idx].deviceName, deviceName, nameLen);
                centralExtDevList[idx].nameLen = nameLen;
                centralExtDevList[idx].vendorId = 0;
                centralExtDevList[idx].productId = 0;
                centralExtDevList[idx].isTargetDevice = 0;
                
                // Check if device name matches target             // ����豸���Ƿ�ƥ��
                if(nameLen > 0 && tmos_memcmp(deviceName, TARGET_DEVICE_NAME, nameLen))
                {
                    centralExtDevList[idx].isTargetDevice = 1;
                    PRINT("Found target device by name: %s\n", deviceName);
                }
            }
        }
        break;

        case GAP_DEVICE_DISCOVERY_EVENT:                           // �豸�����¼�
        {
            uint8_t targetIdx;

            // Find target device by name or VID/PID               // ͨ���豸����VID/PID����Ŀ���豸
            targetIdx = centralFindTargetDevice();

            // Target device not found                             // δ�ҵ�Ŀ���豸
            if(targetIdx == 0xFF)
            {
                PRINT("Target HID device not found, restarting discovery...\n"); // ��ӡδ�ҵ��豸
                centralScanRes = 0;
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE, // ���¿�ʼ�豸����
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
                PRINT("Discovering...\n");
            }

            // Target device found                                 // �ҵ�Ŀ���豸
            else
            {
                targetDeviceIndex = targetIdx;
                PRINT("Target HID device found: %s\n", centralExtDevList[targetIdx].deviceName); // ��ӡ�ҵ��豸
                GAPRole_CentralEstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE, // ��������
                                             DEFAULT_LINK_WHITE_LIST,
                                             centralDevList[targetIdx].addrType,
                                             centralDevList[targetIdx].addr);

                // Start establish link timeout event                // �����������ӳ�ʱ�¼�
                tmos_start_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT, ESTABLISH_LINK_TIMEOUT);
                PRINT("Connecting to HID device...\n");             // ��ӡ��������
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

                // Read device information to verify VID/PID        // ��ȡ�豸��Ϣ��֤VID/PID
                tmos_start_task(centralTaskId, START_READ_DEVICE_INFO_EVT, 800);

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

                PRINT("Connected to HID device...\n");
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
            
            // ֻ����AE00������
            centralAE00SvcStartHdl = centralAE00SvcEndHdl = centralAE10CharHdl = centralCCCDHdl = 0;
            
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
 * @brief   Pairing state callback.                                 // ���״̬�ص�����
 *
 * @return  none                                                    // �޷���ֵ
 */
static void centralPairStateCB(uint16_t connHandle, uint8_t state, uint8_t status)
{
    if(state == GAPBOND_PAIRING_STATE_STARTED)                      // �������Կ�ʼ״̬
    {
        PRINT("Pairing started:%d\n", status);                      // ��ӡ��Կ�ʼ
    }
    else if(state == GAPBOND_PAIRING_STATE_COMPLETE)               // �����������״̬
    {
        if(status == SUCCESS)                                       // �����Գɹ�
        {
            PRINT("Pairing success\n");                             // ��ӡ��Գɹ�
        }
        else
        {
            PRINT("Pairing fail\n");                                // ��ӡ���ʧ��
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BONDED)                 // ������Ѱ�״̬
    {
        if(status == SUCCESS)                                       // ����󶨳ɹ�
        {
            PRINT("Bonding success\n");                             // ��ӡ�󶨳ɹ�
        }
    }
    else if(state == GAPBOND_PAIRING_STATE_BOND_SAVED)             // ����ǰ󶨱���״̬
    {
        if(status == SUCCESS)                                       // �������ɹ�
        {
            PRINT("Bond save success\n");                           // ��ӡ�󶨱���ɹ�
        }
        else
        {
            PRINT("Bond save failed: %d\n", status);                // ��ӡ�󶨱���ʧ��
        }
    }
}

/*********************************************************************
 * @fn      centralPasscodeCB
 *
 * @brief   Passcode callback.                                      // ����ص�����
 *
 * @return  none                                                    // �޷���ֵ
 */
static void centralPasscodeCB(uint8_t *deviceAddr, uint16_t connectionHandle,
                              uint8_t uiInputs, uint8_t uiOutputs)
{
    uint32_t passcode;                                              // �������

    // Create random passcode                                       // �����������
    passcode = tmos_rand();
    passcode %= 1000000;                                           // ��������Ϊ6λ��
    // Display passcode to user                                    // ��ʾ������û�
    if(uiOutputs != 0)
    {
        PRINT("Passcode:%06d\n", (int)passcode);                   // ��ӡ6λ����
    }
    // Send passcode response                                      // ����������Ӧ
    GAPBondMgr_PasscodeRsp(connectionHandle, SUCCESS, passcode);
}

/*********************************************************************
 * @fn      centralStartDiscovery
 *
 * @brief   Start service discovery                               // ��ʼ������
 *
 * @return  none                                                 // �޷���ֵ
 */
static void centralStartDiscovery(void)
{
    // ֻ����AE00������ȫ����HID
    uint8_t uuid[ATT_BT_UUID_SIZE] = {LO_UINT16(AE00_SERVICE_UUID),
                                      HI_UINT16(AE00_SERVICE_UUID)}; // AE00����UUID

    // Initialize AE00 service handles only                       // ֻ��ʼ��AE00������
    centralAE00SvcStartHdl = centralAE00SvcEndHdl = centralAE10CharHdl = centralCCCDHdl = 0;

    centralDiscState = BLE_DISC_STATE_SVC;                        // ���÷���״̬Ϊ������

    // Discovery AE00 service only                                // ֻ����AE00����
    PRINT("��ʼ����AE00����...\n");
    GATT_DiscPrimaryServiceByUUID(centralConnHandle,
                                  uuid,
                                  ATT_BT_UUID_SIZE,
                                  centralTaskId);
}

/*********************************************************************
 * @fn      centralGATTDiscoveryEvent
 *
 * @brief   Process GATT discovery event                           // ����GATT�����¼�
 *
 * @return  none                                                   // �޷���ֵ
 */
static void centralGATTDiscoveryEvent(gattMsgEvent_t *pMsg)
{
    if(centralDiscState == BLE_DISC_STATE_SVC)                      // �����AE00������״̬
    {
        // Service found, store handles                             // �ҵ����񣬴洢���
        if(pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
           pMsg->msg.findByTypeValueRsp.numInfo > 0)
        {
            centralAE00SvcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
            centralAE00SvcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0);
            PRINT("Found AE00 Service handle : %x ~ %x \n", centralAE00SvcStartHdl, centralAE00SvcEndHdl);
        }
        // If procedure complete                                    // ����������
        if((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            if(centralAE00SvcStartHdl != 0)                         // ����ҵ�AE00����
            {
                // Discover all characteristics in AE00 service    // ����AE00�����е���������
                centralDiscState = BLE_DISC_STATE_CHAR;             // ����״̬Ϊ��������
                GATT_DiscAllChars(centralConnHandle, centralAE00SvcStartHdl, centralAE00SvcEndHdl, centralTaskId);
            }
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CHAR)                // �����AE00��������״̬
    {
        // Process discovered characteristics                       // �����ֵ�����
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            uint8_t i;
            uint8_t *pData = pMsg->msg.readByTypeRsp.pDataList;
            uint8_t pairLen = pMsg->msg.readByTypeRsp.len;
            
            // Process each characteristic found                    // ����ÿ�����ֵ�����
            for(i = 0; i < pMsg->msg.readByTypeRsp.numPairs; i++)
            {
                uint16_t handle = BUILD_UINT16(pData[0], pData[1]);
                uint16_t uuid = BUILD_UINT16(pData[5], pData[6]);
                
                PRINT("Found Characteristic - Handle: 0x%04X, UUID: 0x%04X\n", handle, uuid);
                
                if(uuid == AE10_CHAR_UUID)                          // ����AE10����
                {
                    centralAE10CharHdl = handle;
                    PRINT("�ҵ�AE10�������: 0x%04X\n", handle);
                }
                
                pData += pairLen;
            }
        }
        
        if((pMsg->method == ATT_READ_BY_TYPE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            // AE00 characteristics discovery completed            // AE00�����������
            PRINT("=== AE00 Service Discovery Summary ===\n");
            if(centralAE10CharHdl != 0)
            {
                PRINT("AE10�������: 0x%04X\n", centralAE10CharHdl);
                PRINT("AE00��������ɣ�Ready to send data using command 0x3A\n");
            }
            else
            {
                PRINT("Error: δ�ҵ�AE10����\n");
            }
            
            centralDiscState = BLE_DISC_STATE_IDLE;                 // ����״̬Ϊ����
            centralProcedureInProgress = FALSE;                     // ������������б�־
        }
    }
}

/*********************************************************************
 * @fn      centralAddDeviceInfo
 *
 * @brief   Add a device to the device discovery result list       // ����豸���豸���ֽ���б�
 *
 * @return  none                                                   // �޷���ֵ
 */
static void centralAddDeviceInfo(uint8_t *pAddr, uint8_t addrType)
{
    uint8_t i;

    // If result count not at max                                  // ����������δ�ﵽ���ֵ
    if(centralScanRes < DEFAULT_MAX_SCAN_RES)
    {
        // Check if device is already in scan results              // ����豸�Ƿ�����ɨ������
        for(i = 0; i < centralScanRes; i++)
        {
            if(tmos_memcmp(pAddr, centralDevList[i].addr, B_ADDR_LEN))
            {
                return;
            }
        }
        // Add addr to scan result list                           // ��ӵ�ַ��ɨ�����б�
        tmos_memcpy(centralDevList[centralScanRes].addr, pAddr, B_ADDR_LEN);
        centralDevList[centralScanRes].addrType = addrType;
        // Increment scan result count                            // ����ɨ��������
        centralScanRes++;
        // Display device addr                                    // ��ʾ�豸��ַ
//        PRINT("Device %d - Addr %x %x %x %x %x %x \n", centralScanRes,
//              centralDevList[centralScanRes - 1].addr[0],
//              centralDevList[centralScanRes - 1].addr[1],
//              centralDevList[centralScanRes - 1].addr[2],
//              centralDevList[centralScanRes - 1].addr[3],
//              centralDevList[centralScanRes - 1].addr[4],
//              centralDevList[centralScanRes - 1].addr[5]);
    }
}

/*********************************************************************
 * @fn      centralParseAdvertData
 *
 * @brief   Parse advertising data to extract device name          // �����㲥������ȡ�豸��
 *
 * @param   pData - advertising data                              // pData - �㲥����
 * @param   dataLen - data length                                 // dataLen - ���ݳ���
 * @param   deviceName - buffer to store device name             // deviceName - �洢�豸���Ļ�����
 * @param   nameLen - pointer to store name length               // nameLen - �洢���Ƴ��ȵ�ָ��
 *
 * @return  TRUE if name found, FALSE otherwise                   // �ҵ����Ʒ���TRUE������FALSE
 */
static uint8_t centralParseAdvertData(uint8_t *pData, uint8_t dataLen, uint8_t *deviceName, uint8_t *nameLen)
{
    uint8_t i = 0;
    *nameLen = 0;
    
    while(i < dataLen)
    {
        uint8_t len = pData[i];
        uint8_t type = pData[i + 1];
        
        if(len == 0) break;
        
        // Check for Complete Local Name (0x09) or Shortened Local Name (0x08)
        if(type == 0x09 || type == 0x08)
        {
            *nameLen = len - 1;
            if(*nameLen > 31) *nameLen = 31; // Limit name length
            tmos_memcpy(deviceName, &pData[i + 2], *nameLen);
            deviceName[*nameLen] = '\0';
            return TRUE;
        }
        
        i += len + 1;
    }
    
    return FALSE;
}

/*********************************************************************
 * @fn      centralFindTargetDevice
 *
 * @brief   Find target device in scan results                    // ��ɨ�����в���Ŀ���豸
 *
 * @return  Device index if found, 0xFF if not found             // �ҵ������豸���������򷵻�0xFF
 */
static uint8_t centralFindTargetDevice(void)
{
    uint8_t i;
    
    // First priority: exact device name match
    for(i = 0; i < centralScanRes; i++)
    {
        if(centralExtDevList[i].isTargetDevice)
        {
            PRINT("Found target device by exact name match: %s\n", centralExtDevList[i].deviceName);
            return i;
        }
    }
    
    // Second priority: partial name match containing "HID"
    for(i = 0; i < centralScanRes; i++)
    {
        if(centralExtDevList[i].nameLen > 0)
        {
            // Simple search for "HID" in device name
            char *nameStr = (char*)centralExtDevList[i].deviceName;
            if(strstr(nameStr, "HID") != NULL)
            {
                PRINT("Found HID device by partial name match: %s\n", centralExtDevList[i].deviceName);
                centralExtDevList[i].isTargetDevice = 1; // Mark as target
                return i;
            }
        }
    }
    
    // Debug: print all discovered devices
    PRINT("No HID device found. Discovered devices:\n");
    for(i = 0; i < centralScanRes; i++)
    {
        if(centralExtDevList[i].nameLen > 0)
        {
            PRINT("  Device %d: %s\n", i, centralExtDevList[i].deviceName);
        }
        else
        {
            PRINT("  Device %d: (No name)\n", i);
        }
    }
    
    return 0xFF; // Not found
}

/*********************************************************************
 * @fn      centralReadDeviceInfo
 *
 * @brief   Read device information service to get VID/PID        // ��ȡ�豸��Ϣ�����ȡVID/PID
 *
 * @return  none                                                  // �޷���ֵ
 */
static void centralReadDeviceInfo(void)
{
    // This is a simplified implementation
    // In a full implementation, you would:
    // 1. Discover Device Information Service (0x180A)
    // 2. Read PnP ID characteristic (0x2A50) which contains VID/PID
    // For now, we'll assume the device name match is sufficient
    
    if(targetDeviceIndex != 0xFF)
    {
        // Simulate reading VID/PID (in real implementation, read from GATT)
        centralExtDevList[targetDeviceIndex].vendorId = TARGET_VENDOR_ID;
        centralExtDevList[targetDeviceIndex].productId = TARGET_PRODUCT_ID;
        
        PRINT("Device Info - VID: 0x%04X, PID: 0x%04X\n", 
              centralExtDevList[targetDeviceIndex].vendorId,
              centralExtDevList[targetDeviceIndex].productId);
    }
    
    centralProcedureInProgress = FALSE;
}

/*********************************************************************
 * @fn      SendHIDTestData
 *
 * @brief   Send 20 bytes test data to HID device                 // ��HID�豸����20�ֽڲ�������
 *
 * @return  TRUE if successful, FALSE otherwise                   // �ɹ�����TRUE������FALSE
 */
BOOL SendHIDTestData(void)
{
    if(centralConnHandle == GAP_CONNHANDLE_INIT || centralState != BLE_STATE_CONNECTED)
    {
        PRINT("�豸δ���ӣ��޷���������\n");
        return FALSE;
    }

    if(centralAE10CharHdl == 0)
    {
        PRINT("����AE10���������Ч���޷���������\n");
        return FALSE;
    }

    // ֱ�ӹ���20�ֽ����ݰ�������ҪReport ID
    attWriteReq_t req;
    req.cmd = FALSE;                                      // ��������
    req.sig = FALSE;                                      // ����ǩ��
    req.handle = centralAE10CharHdl;                      // ʹ��AE10�������
    req.len = 20;                                         // ֻ����20�ֽ�����
    
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0);
    if(req.pValue != NULL)
    {
        // ���20�ֽڵ�������: 01, 02, 03 ... 20
        for(uint8_t i = 0; i < 20; i++) 
        {
            req.pValue[i] = i + 1;
        }
        
        PRINT("����20�ֽ����ݵ�AE10���� (���: 0x%04X): ", req.handle);
        for(uint8_t i = 0; i < req.len; i++) 
        {
            PRINT("%02X ", req.pValue[i]);
        }
        PRINT("\n");

        if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS)
        {
            PRINT("���ݷ�������ɹ�\n");
            return TRUE;
        }
        else
        {
            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
            PRINT("���ݷ�������ʧ��\n");
            return FALSE;
        }
    }
    else
    {
        PRINT("�����ڴ����ʧ��\n");
        return FALSE;
    }
}

/************************ endfile @ central **************************/
