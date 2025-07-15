# CH582M HID������Ŀ

����Ŀ�ǻ����ߺ��CH582M��HID�ٿ���Ŀ����Ŀ��CH582M��Ϊ���豸��BLE Central�������Է��ֲ�����ָ����HID�����豸��

## ��Ŀ�ṹ

```
ch582test/
������ APP/                          # Ӧ�ò����
��   ������ central_main.c           # ��������ڣ�ϵͳ��ʼ��
��   ������ central.c/central.h      # BLE Central���Ĺ���ʵ��
��   ������ uart_cmd.c/uart_cmd.h    # UART�����ģ��
��   ������ key.c/key.h              # ��������ģ��(״̬��ʵ��)
��   ������ ws2812.c/ws2812.h        # WS2812 LED״ָ̬ʾģ��
������ Profile/                     # BLE�����ļ�
��   ������ include/gattprofile.h    # GATT�������������
������ obj/                         # ��������ļ�
������ Ӳ��������/                   # CH582оƬ�ײ�����
```

## ���Ĺ���ģ��

### 1. BLE Centralģ�� (central.c)
- **�����豸����**: ͨ���豸��"HID-LongWang"�Զ�ʶ��Ŀ��HID�豸
- **VendorID/ProductID��֤**: ���Ӻ���֤VID(0x08D8)/PID(0x0001)ƥ��
- **�Զ����ӹ���**: ʧ��ʱ�Զ�����ɨ������
- **GATT������**: �Զ�����HID����(0x1812)���������
- **���ݴ���**: ֧��20�ֽ�HID���ݷ��ͺͽ���

### 2. UART����ģ�� (uart_cmd.c)
֧�����´������
- `0x31`: �л�CH���ڿ���
- `0x32`: �л�ESP��¼�ڿ���  
- `0x33`: �л�����ESP��¼���ڿ���
- `0x34`: �л�ESP����1��־����
- `0x35`: �л��¶ȴ���������
- `0x38`: �������������¼�
- `0x39`: ϵͳ��λ
- **`0x3A`: ����HID�������� (����)**

### 3. ��������ģ�� (key.c)
- �ж�+��ʱ���İ���״̬��
- ֧�ֵ�����˫�����������
- ��Դ�������(3.3V/12V�л�)

### 4. LEDָʾģ�� (ws2812.c)
- SPI����WS2812�ƴ�
- ��ͬ״̬�Ŀ��ӻ�ָʾ
- ���ȿɵ���

## HID�豸ƥ�����

��Ŀ֧������ƥ�䷽ʽ��

### ��Ҫƥ�䷽ʽ���豸��ƥ��
- ɨ������н����㲥����
- ƥ���豸����`"HID-LongWang"`
- ���ȼ���ߣ����ּ�����

### ������֤��VendorID/ProductID
- ���Ӻ��ȡDevice Information Service
- ��֤VendorID: `0x08D8`
- ��֤ProductID: `0x0001`

## HIDͨ��Э��

### ����ͨ��
- **Report ID 1**: (HID���������CH582) (20�ֽ���Ч�غ�)
- **Report ID 2**: (����CH582��HID����) (20�ֽ���Ч�غ�)

### �������ݸ�ʽ
���Ͳ����������ݣ�`01 02 03 04 ... 14` (20�������ֽ�)

## ʹ�÷���

### 1. �豸����
1. �ϵ��ϵͳ�Զ���ʼɨ��HID�豸
2. ������Ϊ"HID-LongWang"���豸���Զ�����
3. ���ӳɹ�����з����ֺ���������

### 2. ���Ͳ�������
ͨ�����ڷ������� `0x3A` ����20�ֽڲ������ݷ���

### 3. ״̬���
- ͨ��������־�鿴����״̬�����ݴ������
- WS2812 LEDָʾ��ͬ�Ĺ���״̬

## �ؼ�����

- ? **�����豸ʶ��**: �����豸���Զ�ƥ�䣬����Ӳ����MAC��ַ
- ? **˫����֤����**: �豸��+VID/PID˫��ȷ��
- ? **�Զ�����**: ����ʧ�ܻ�Ͽ����Զ�����ɨ��
- ? **������HID֧��**: ��׼HID����(0x1812)ͨ��
- ? **���ڿ��ƽӿ�**: �����ⲿϵͳ����
- ? **״̬���ӻ�**: LEDָʾ���ֹ���״̬

## �������

- **оƬ**: �ߺ�CH582M
- **Э��**: BLE 5.0 Centralģʽ
- **HID����**: UUID 0x1812
- **���ݳ���**: 20�ֽ���Ч�غ�
- **���Ӳ���**: �����ü���ͳ�ʱʱ��

## ��Ŀ�ṹ����

�����Ҷ�������Ŀ�������Ķ�������һ������CH582MоƬ��BLE Central�������豸����Ŀ���������½ṹ��

### ����ģ��ܹ�

```
<code_block_to_apply_changes_from>
```

### ����ģ�����

1. **BLE Centralģ��** (`central.c`)
   - �豸ɨ���뷢��
   - �Զ�����Ŀ��HID�豸
   - GATT������
   - ����ֵ��д����
   - ���ӹ�����쳣����

2. **UART����ģ��** (`uart_cmd.c`)
   - �����ⲿ��������
   - ֧�ֶ��ֿ��ؿ���(0x31-0x39)
   - ���Ź���λ����

3. **��������ģ��** (`key.c`)
   - �ж�+��ʱ���İ���״̬��
   - ֧�ֵ�����˫�����������
   - ��Դ�������

4. **LEDָʾģ��** (`ws2812.c`)
   - SPI����WS2812�ƴ�
   - ״̬���ӻ�ָʾ
   - ���ȿɵ���

## HID���ֺ����ӹ���ʵ�ַ���

### 1. HID�豸��������

```c
// �� central.c �е��豸�������̣�
GAP_DEVICE_INIT_DONE_EVENT -> 
GAPRole_CentralStartDiscovery() -> 
GAP_DEVICE_INFO_EVENT -> centralAddDeviceInfo() ->
GAP_DEVICE_DISCOVERY_EVENT -> ���Ŀ���ַƥ��
```

**Ŀ���豸���ã�**
```c
// Ԥ���HID�豸��ַ
static uint8_t PeerAddrDef[B_ADDR_LEN] = {0x03, 0x03, 0x03, 0xE4, 0xC2, 0x84};
```

### 2. HID�豸��������

```c
// ���ӽ�������
GAPRole_CentralEstablishLink() -> 
GAP_LINK_ESTABLISHED_EVENT -> 
centralStartDiscovery() -> // ������
GATT_DiscPrimaryServiceByUUID() -> // ����������
GATT_DiscAllChars() -> // ������������
GATT_ReadUsingCharUUID() // ����CCCD
```

**ʹ�õķ�����Ϣ��**
- ����UUID: `0x1812` (HID Service)
- ����UUID: `0x2A22` (Boot Keyboard Input Report)
- ֧��֪ͨ����(CCCD)

### 3. ���ݴ������

��ǰ��Ŀͨ��GATTЭ����HID�豸ͨ�ţ�
- **��������**: ͨ��ATT_HANDLE_VALUE_NOTI�¼�����HID�豸���͵�����
- **��������**: ͨ��GATT_WriteCharValue()���������д������

## ��HID���跢��20�ֽڲ������ݵ�ʵ�ַ���

### ��������

�������������Ҫ��HID���跢������Ϊ`01,02,03,...,20`��20�ֽڲ������ݡ��������д���ܹ����ҽ�������ʵ�ַ�����

### 1. ���HID���ݷ��ͺ���

```c
// ��central.c������µķ��ͺ���
bool SendHIDTestData(void)
{
    if(centralState != BLE_STATE_CONNECTED || centralCharHdl == 0) 
    {
        PRINT("HID�豸δ���ӻ����������Ч\n");
        return false;
    }
    
    if(centralProcedureInProgress == TRUE) 
    {
        PRINT("GATT�������ڽ����У����Ժ�����\n");
        return false;
    }

    // ׼��20�ֽڲ������� (01, 02, 03, ..., 20)
    attWriteReq_t req;
    req.cmd = FALSE;
    req.sig = FALSE;
    req.handle = centralCharHdl;  // ʹ�÷��ֵ��������
    req.len = 20;  // 20�ֽ����ݳ���
    
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0);
    if(req.pValue != NULL) 
    {
        // ���������� 01, 02, 03, ..., 20
        for(uint8_t i = 0; i < 20; i++) 
        {
            req.pValue[i] = i + 1;
        }
        
        // �������ݵ�HID�豸
        if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) 
        {
            centralProcedureInProgress = TRUE;
            PRINT("20�ֽڲ������ݷ��ͳɹ�\n");
            return true;
        }
        else 
        {
            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
            PRINT("���ݷ���ʧ��\n");
            return false;
        }
    }
    else 
    {
        PRINT("�ڴ����ʧ��\n");
        return false;
    }
}
```

### 2. ��Ӵ����¼�

```c
// ��central.h��������¼�����
#define START_HID_TEST_DATA_EVT    0x0400

// ��central.c���¼������������
if(events & START_HID_TEST_DATA_EVT) 
{
    SendHIDTestData();
    return (events ^ START_HID_TEST_DATA_EVT);
}
```

### 3. ���÷�ʽ

#### ��ʽ1��ͨ��UART�����
```c
// ��uart_cmd.c������µ������
if (uart_rx_buffer[0] == 0x3A)  // ����0x3A�������ڷ���HID��������
{
    PRINT("����HID��������\n");
    tmos_set_event(centralTaskId, START_HID_TEST_DATA_EVT);
}
```

#### ��ʽ2��ͨ����������
```c
// ��key.c�ĳ����¼������
if(events & KEY_EVENT_LONG_PRESS)
{
    PRINT("��������HID�������ݷ���\n");
    tmos_set_event(centralTaskId, START_HID_TEST_DATA_EVT);
    return (events ^ KEY_EVENT_LONG_PRESS);
}
```

#### ��ʽ3����ʱ�Զ�����
```c
// ���ӳɹ���������ʱ����
if(pEvent->gap.hdr.status == SUCCESS)
{
    // ... �������Ӵ������ ...
    
    // ����5����Զ����Ͳ�������
    tmos_start_task(centralTaskId, START_HID_TEST_DATA_EVT, 8000); // 5�����
}
```

### 4. ���ݷ���ȷ��

���ͽ������`centralProcessGATTMsg()`������д����Ӧ������ȷ�ϣ�

```c
else if((pMsg->method == ATT_WRITE_RSP) || ...)
{
    if(pMsg->method == ATT_ERROR_RSP)
    {
        PRINT("HID���ݷ���ʧ��: %x\n", status);
    }
    else
    {
        PRINT("HID�������ݷ��ͳɹ�ȷ��\n");
    }
    centralProcedureInProgress = FALSE;
}
```

### ʹ��˵��

1. **����HID�豸**��ϵͳ���Զ�ɨ�貢�������õ�HID�豸��ַ
2. **�������ݷ���**��ͨ�����ڷ���`0x3A`����򳤰�����
3. **ȷ�Ϸ��ͽ��**��ͨ����������鿴����״̬

���������ȫ�������еĴ���ܹ����������޸ģ�ֻ��Ҫ��Ӷ�Ӧ���¼�����ͺ�������ʵ����HID���跢��20�ֽڲ������ݵĹ��ܡ�



