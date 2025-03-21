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

// Discovery states                                                // ����״̬ö��
enum
{
    BLE_DISC_STATE_IDLE,                                         // ����״̬
    BLE_DISC_STATE_SVC,                                          // ������״̬
    BLE_DISC_STATE_CHAR,                                         // ��������״̬
    BLE_DISC_STATE_CCCD                                          // CCCD����״̬
};

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
static uint8_t centralTaskId;                                        // �����豸����ID

// Number of scan results                                            
static uint8_t centralScanRes;                                       // ɨ��������

// Scan result list                                                  
static gapDevRec_t centralDevList[DEFAULT_MAX_SCAN_RES];            // ɨ�����б�

// Peer device address                                               
static uint8_t PeerAddrDef[B_ADDR_LEN] = {0x03, 0x03, 0x03, 0xE4, 0xC2, 0x84}; // �Զ��豸��ַ

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

// Discovered service start and end handle                           
static uint16_t centralSvcStartHdl = 0;                             // ���ֵķ�����ʼ���
static uint16_t centralSvcEndHdl = 0;                               // ���ֵķ���������

// Discovered characteristic handle                                   
static uint16_t centralCharHdl = 0;                                 // ���ֵ��������

// Discovered Client Characteristic Configuration handle              
static uint16_t centralCCCDHdl = 0;                                 // ���ֵĿͻ����������þ��

// Value to write                                                    
static uint8_t centralCharVal = 0x5A;                               // Ҫд���ֵ

// Value read/write toggle                                           
static uint8_t centralDoWrite = TRUE;                               // ��/д�л���־

// GATT read/write procedure state                                   
static uint8_t centralProcedureInProgress = FALSE;                  // GATT��/д����״̬

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
                // Do a write                                         // ִ��д����
                attWriteReq_t req;

                req.cmd = FALSE;                                      // ��������
                req.sig = FALSE;                                      // ����ǩ��
                req.handle = centralCharHdl;                          // �����������

                req.len = 1;                                          // д�볤��Ϊ1
                req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0); // �����ڴ�
                printf("READ_OR_WRITE_handle = %x\n" ,req.handle);    // ��ӡ���ֵ
                if(req.pValue != NULL)                                // ����ڴ����ɹ�
                {
                    *req.pValue = centralCharVal;                     // ����д��ֵ

                    if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // д������ֵ
                    {
                        centralProcedureInProgress = TRUE;            // ���ò��������б�־
                        centralDoWrite = !centralDoWrite;             // �л���д��־
                        tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY); // ������һ�ζ�д
                    }
                    else
                    {
                        GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ); // �ͷ��ڴ�
                    }
                }
            }
            else
            {
                // Do a read                                          // ִ�ж�����
                attReadReq_t req;

                req.handle = centralCharHdl;                          // �����������
                if(GATT_ReadCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) // ��ȡ����ֵ
                {
                    centralProcedureInProgress = TRUE;                // ���ò��������б�־
                    centralDoWrite = !centralDoWrite;                 // �л���д��־
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

            PRINT("Write Error: %x\n", status);                      // ��ӡд�����
        }
        else
        {
            // Write success                                         // д��ɹ�
            PRINT("Write success \n");                              // ��ӡд��ɹ�
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
            // Add device to list                                   // ����豸���б�
            centralAddDeviceInfo(pEvent->deviceInfo.addr, pEvent->deviceInfo.addrType);
        }
        break;

        case GAP_DEVICE_DISCOVERY_EVENT:                           // �豸�����¼�
        {
            uint8_t i;

            // See if peer device has been discovered                // ����Ƿ���Ŀ���豸
            for(i = 0; i < centralScanRes; i++)
            {
                if(tmos_memcmp(PeerAddrDef, centralDevList[i].addr, B_ADDR_LEN))
                    break;
            }

            // Peer device not found                                // δ�ҵ�Ŀ���豸
            if(i == centralScanRes)
            {
                PRINT("Device not found...\n");                     // ��ӡδ�ҵ��豸
                centralScanRes = 0;
                GAPRole_CentralStartDiscovery(DEFAULT_DISCOVERY_MODE, // ���¿�ʼ�豸����
                                              DEFAULT_DISCOVERY_ACTIVE_SCAN,
                                              DEFAULT_DISCOVERY_WHITE_LIST);
                PRINT("Discovering...\n");
            }

            // Peer device found                                    // �ҵ�Ŀ���豸
            else
            {
                PRINT("Device found...\n");                         // ��ӡ�ҵ��豸
                GAPRole_CentralEstablishLink(DEFAULT_LINK_HIGH_DUTY_CYCLE, // ��������
                                             DEFAULT_LINK_WHITE_LIST,
                                             centralDevList[i].addrType,
                                             centralDevList[i].addr);

                // Start establish link timeout event                // �����������ӳ�ʱ�¼�
                tmos_start_task(centralTaskId, ESTABLISH_LINK_TIMEOUT_EVT, ESTABLISH_LINK_TIMEOUT);
                PRINT("Connecting...\n");                           // ��ӡ��������
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
 * @brief   Start service discovery.                               // ��ʼ������
 *
 * @return  none                                                   // �޷���ֵ
 */
static void centralStartDiscovery(void)
{
    uint8_t uuid[ATT_BT_UUID_SIZE] = {LO_UINT16(SIMPLEPROFILE_SERV_UUID),
                                      HI_UINT16(SIMPLEPROFILE_SERV_UUID)}; // ����UUID

    // Initialize cached handles                                    // ��ʼ������ľ��
    centralSvcStartHdl = centralSvcEndHdl = centralCharHdl = 0;

    centralDiscState = BLE_DISC_STATE_SVC;                         // ���÷���״̬Ϊ������

    // Discovery simple BLE service                                // ���ּ�BLE����
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
    attReadByTypeReq_t req;                                        // ��ȡ����ṹ��
    if(centralDiscState == BLE_DISC_STATE_SVC)                     // ����Ƿ�����״̬
    {
        // Service found, store handles                            // �ҵ����񣬴洢���
        if(pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
           pMsg->msg.findByTypeValueRsp.numInfo > 0)
        {
            centralSvcStartHdl = ATT_ATTR_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0); // ���������ʼ���
            centralSvcEndHdl = ATT_GRP_END_HANDLE(pMsg->msg.findByTypeValueRsp.pHandlesInfo, 0); // �������������

            // Display Profile Service handle range                 // ��ʾ�����ļ���������Χ
            PRINT("Found Profile Service handle : %x ~ %x \n", centralSvcStartHdl, centralSvcEndHdl);
        }
        // If procedure complete                                   // ����������
        if((pMsg->method == ATT_FIND_BY_TYPE_VALUE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            if(centralSvcStartHdl != 0)                            // ����ҵ�����
            {
                // Discover characteristic                         // ��������
                centralDiscState = BLE_DISC_STATE_CHAR;            // ����״̬Ϊ��������
                req.startHandle = centralSvcStartHdl;              // ������ʼ���
                req.endHandle = centralSvcEndHdl;                  // ���ý������
                req.type.len = ATT_BT_UUID_SIZE;                   // ����UUID����
                req.type.uuid[0] = LO_UINT16(SIMPLEPROFILE_CHAR1_UUID); // ��������UUID
                req.type.uuid[1] = HI_UINT16(SIMPLEPROFILE_CHAR1_UUID);

//                GATT_ReadUsingCharUUID(centralConnHandle, &req, centralTaskId);
                GATT_DiscAllChars(centralConnHandle, centralSvcStartHdl, centralSvcEndHdl, centralTaskId); // ������������
            }
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CHAR)              // �������������״̬
    {
        // Characteristic found, store handle                      // �ҵ��������洢���
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            centralCharHdl = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[0], // �����������
                                          pMsg->msg.readByTypeRsp.pDataList[1]);

            // Start do read or write                             // ��ʼ��д����
            tmos_start_task(centralTaskId, START_READ_OR_WRITE_EVT, DEFAULT_READ_OR_WRITE_DELAY);

            // Display Characteristic 1 handle                     // ��ʾ����1���
            PRINT("Found Characteristic 1 handle : %x \n", centralCharHdl);
        }
        if((pMsg->method == ATT_READ_BY_TYPE_RSP &&
            pMsg->hdr.status == bleProcedureComplete) ||
           (pMsg->method == ATT_ERROR_RSP))
        {
            // Discover characteristic                            // ��������
            centralDiscState = BLE_DISC_STATE_CCCD;               // ����״̬ΪCCCD����
            req.startHandle = centralSvcStartHdl;                 // ������ʼ���
            req.endHandle = centralSvcEndHdl;                     // ���ý������
            req.type.len = ATT_BT_UUID_SIZE;                      // ����UUID����
            req.type.uuid[0] = LO_UINT16(GATT_CLIENT_CHAR_CFG_UUID); // ����CCCD UUID
            req.type.uuid[1] = HI_UINT16(GATT_CLIENT_CHAR_CFG_UUID);

            GATT_ReadUsingCharUUID(centralConnHandle, &req, centralTaskId); // ��ȡCCCD
        }
    }
    else if(centralDiscState == BLE_DISC_STATE_CCCD)             // �����CCCD����״̬
    {
        // Characteristic found, store handle                     // �ҵ��������洢���
        if(pMsg->method == ATT_READ_BY_TYPE_RSP &&
           pMsg->msg.readByTypeRsp.numPairs > 0)
        {
            centralCCCDHdl = BUILD_UINT16(pMsg->msg.readByTypeRsp.pDataList[0], // ����CCCD���
                                          pMsg->msg.readByTypeRsp.pDataList[1]);
            centralProcedureInProgress = FALSE;                   // ������������б�־

            // Start do write CCCD                                // ��ʼдCCCD
            tmos_start_task(centralTaskId, START_WRITE_CCCD_EVT, DEFAULT_WRITE_CCCD_DELAY);

            // Display Characteristic 1 handle                    // ��ʾCCCD���
            PRINT("Found client characteristic configuration handle : %x \n", centralCCCDHdl);
        }
        centralDiscState = BLE_DISC_STATE_IDLE;                  // ����״̬Ϊ����
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

/************************ endfile @ central **************************/
