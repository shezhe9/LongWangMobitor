#include "CONFIG.h"
#include "key.h"
#include "ws2812.h"
// ע�⣺��Ҫȷ����������ͷ�ļ���������CH582M��ʱ����ض���
// #include "CH58x_common.h"  

#define SINGLE_CLICK_TIME    300  // �������ʱ������ms��
#define DOUBLE_CLICK_TIME    600  // ˫�����ʱ������ms��
#define LONG_PRESS_TIME      1500 // ����ʱ�䣨ms��



typedef enum {
    BUTTON_IDLE,              // ����̬
    BUTTON_PRESSED,           // ����̬
    BUTTON_DOUBLE_CLICK_WAIT  // �ȴ�˫��̬
} ButtonState;

volatile ButtonState buttonState = BUTTON_IDLE;  // ��ť״̬��
volatile KeyEvent keyEvent = KEY_EVENT_NONE;     // ��ǰ�����¼�
 uint8_t keyTaskId = 0xFF;                 // TMOS����ID�����ڷ��Ͱ����¼�

// ʱ����ṹ�壨ʹ��RTC�Ĵ�����
typedef struct {
    uint16_t day;     // �����ۼƣ�0~65535�죩
    uint16_t sec2;    // �������
    uint16_t t32k;    // 32KHzʱ�Ӽ���
} TimeStamp32;

// ����ȫ��ʱ�����
volatile TimeStamp32 buttonPressTime = {0};    // ��������ʱ���
volatile TimeStamp32 buttonReleaseTime = {0};  // �����ͷ�ʱ���
volatile TimeStamp32 currentTime = {0};        // ��ǰϵͳʱ���

// ȫ�ֱ�����¼��ǰ����ģʽ
static uint8_t currentTriggerMode = GPIO_ITMode_FallEdge;

/**
 * @brief ��ȡ��ǰRTCʱ���
 * 
 * @param timestamp ʱ����ṹ��ָ��
 */
void GetTimeStamp(volatile TimeStamp32 *timestamp)
{
    timestamp->day = R32_RTC_CNT_DAY & 0x3FFF;
    timestamp->sec2 = R16_RTC_CNT_2S;
    timestamp->t32k = R16_RTC_CNT_32K;
}

/**
 * @brief ��������ʱ����Ĳ�ֵ����λ�����룩
 * 
 * @param new ��ʱ���
 * @param old ��ʱ���
 * @return int32_t ʱ�����룩
 */
int32_t TimeDiff(volatile TimeStamp32 *new, volatile TimeStamp32 *old)
{
    int32_t diff = 0;
    diff += (new->day - old->day) * 86400000L;    // ��ת����
    diff += (new->sec2 - old->sec2) * 2000;       // 2��ת����
    diff += (new->t32k - old->t32k) * 30/1000;         // 32K����ת���루Լ30.5usÿ��������
    return diff;
}

void print_timestamp(TimeStamp32 *time)
{
    PRINT("R16_RTC_CNT_2S=%d  ",time->sec2) ;
    PRINT("32K=%d \n",time->t32k) ;
}

int32_t pressDuration=0;
int8_t  unkonw_sate=0;
/**
 * @brief �����жϴ�����
 * 
 * @note �˺�����PB3����״̬�仯ʱ������
 */
void ButtonHandler(void) 
{
    uint32_t buttonPressed = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1;  // ��ȡPB3״̬��0=�ߵ�ƽδ���£�1=�͵�ƽ���£�
    
    //PRINT("ButtonHandler(), state=%d, pressed=%d\n", buttonState, buttonPressed);

    // ���ݰ���״̬�ʹ�����ʽ����
   
    if((buttonState == BUTTON_IDLE)&&(buttonPressed == 1)) // ����̬�°���
    {
        GetTimeStamp(&buttonPressTime);
        buttonState = BUTTON_PRESSED;
        // ����������ⶨʱ��(1.5s)
        //tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, LONG_PRESS_TIME*1600/1000);
        tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, 2400);
    }
    else if((buttonState == BUTTON_PRESSED)&&(buttonPressed == 0)) // ����̬���ͷ�
    {
        // ȡ����������¼�
        tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);

        GetTimeStamp(&buttonReleaseTime);
        pressDuration = TimeDiff(&buttonReleaseTime, &buttonPressTime);
        
        if(pressDuration < KEY_DEBOUNCE_TIME) // ��������������
        {
            tmos_set_event(keyTaskId, KEY_NOISE_PRESSED);
            buttonState = BUTTON_IDLE;
        }
        else if(pressDuration < SINGLE_CLICK_TIME) // �����ǵ�����˫��
        {
            // ����ȴ�˫��̬
            buttonState = BUTTON_DOUBLE_CLICK_WAIT;
            // ����˫������¼� (600ms��ʱ)
            //tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, DOUBLE_CLICK_TIME*1600/1000); // 600ms �� 0.625ms = 960
            tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, 960); // 600ms �� 0.625ms = 960
        
        }
        else if(pressDuration < LONG_PRESS_TIME) // ����
        {
            tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
            buttonState = BUTTON_IDLE;
        }
        else // �������ɶ�ʱ������
        {
            
            tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);
            tmos_set_event(keyTaskId, BUTTON_PRESSED_OVERTIME_ERR);
            tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
            buttonState = BUTTON_IDLE;
        }
    }
    else if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 1)) // �ȴ�˫��̬���ְ���
    {
        // ȡ��˫������¼�
        tmos_stop_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK);

        GetTimeStamp(&currentTime);
        int32_t timeSinceRelease = TimeDiff(&currentTime, &buttonReleaseTime);
        
        if(timeSinceRelease < DOUBLE_CLICK_TIME) // ˫������
        {
            // ����˫���¼�
            tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK);
            // �ص�����̬
            buttonState = BUTTON_IDLE;
        }
        else // ��ʱ������ ��Ӧ��˫�������¼�˫������ɵ���
        {
            tmos_set_event(keyTaskId, DOUBULE_PRESSED_OVERTIME_ERR);
            buttonState = BUTTON_IDLE;
        }
    }
    else
    {
        unkonw_sate=KEY_STATE_UNKOWN;
        if((buttonState == BUTTON_IDLE)&&(buttonPressed == 0))//����̬ ��ť�ͷŽ��룬������ʱ����
            unkonw_sate= SINGLE_PRESSED_RELASE;
        if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 0))//˫�����ͷŰ�ť
            unkonw_sate= DOUBULE_PRESSED_RELASE;
        tmos_set_event(keyTaskId, unkonw_sate);
        buttonState = BUTTON_IDLE;
    }
        
}

void GPIOB_ITCmd(uint8_t pin, FunctionalState state) {
    if (state == ENABLE) {
        // �����ж�
        R16_PB_INT_EN |= pin; // �������������жϵļĴ���
    } else {
        // �����ж�
        R16_PB_INT_EN &= ~pin; // �������ǽ����жϵļĴ���
    }
}
/**
 * @brief ��ʼ������
 */
void Key_Init(void)
{
    // ע�ᰴ���¼�������
    keyTaskId = TMOS_ProcessEventRegister(Key_ProcessEvent);
    PRINT("������ʼ��: keyTaskId=%d\n", keyTaskId);
    
    //����Ϊ����ģʽ
    PRINT("����Ϊ����ģʽ: CH582_Key_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_Key_Pin);
    GPIOB_ModeCfg(CH582_Key_Pin, GPIO_ModeIN_PU);

    //����Ϊ����ģʽ
    PRINT("����Ϊ����ģʽ: CH582_AutoCheck_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_AutoCheck_Pin);
    GPIOB_ModeCfg(CH582_AutoCheck_Pin, GPIO_ModeIN_PU);

    // ����Ϊ����ģʽ��ʹ������ PB0-15�����ж�
    PRINT("����Ϊ����ģʽ: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_PROG_BOOT_Pin);
    GPIOB_ModeCfg(CH582_PROG_BOOT_Pin, GPIO_ModeIN_PU);
    
    //RB_PIN_INTX������INT24/INT25 ��������ӳ��ѡ��λ Ĭ����0,����1���ܣ�INT24_/25_ӳ�䵽 PB[22]/PB[23]��
    //�ж϶�Ӧ������ӳ�� ����(R16_PIN_ALTERNATE��RB_PIN_INTXλΪ1
    //PRINT("����Ϊ����ģʽ: RB_PIN_INTX(0x%x)=1  \n", RB_PIN_INTX);
    //R16_PIN_ALTERNATE |= RB_PIN_INTX;

    // �����жϣ���ʼ����Ϊ�½��ش���
    PRINT("��ʼ����Ϊ�½��ش���: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ITMode_FallEdge  \n", CH582_PROG_BOOT_Pin);
    GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
    //GPIOB_ITCmd(CH582_PROG_BOOT_Pin, ENABLE); cfg�Ѿ�����
    // ����Ĭ�ϴ���ģʽΪ�½���
    currentTriggerMode = GPIO_ITMode_FallEdge;
    // ����GPIO�ж�
    PFIC_EnableIRQ(GPIO_B_IRQn);
    // ���ó�ʼ״̬
    buttonState = BUTTON_IDLE;
    CH340_CTRL_PIN_INI();
}




void CH340_CTRL_PIN_INI(void)
{
    GPIOB_ModeCfg(EN_CH_Pin, GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_SetBits(EN_CH_Pin); // Ĭ�ϸߵ�ƽ
    GPIOB_ModeCfg(EN_ESP_Pin, GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_SetBits(EN_ESP_Pin); // Ĭ�ϸߵ�ƽ
    GPIOA_ModeCfg(EN_ESP_ME_Pin, GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOA_SetBits(EN_ESP_ME_Pin); // Ĭ�ϸߵ�ƽ
    GPIOB_ModeCfg(EN_ESP_UART1_LOG_Pin, GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_SetBits(EN_ESP_UART1_LOG_Pin); // Ĭ�ϸߵ�ƽ

    GPIOB_ModeCfg(CH582_3V3_Pin, GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_ResetBits(CH582_3V3_Pin); // Ϊ�͵�ƽ
    
    GPIOB_ModeCfg(CH582_12V_Pin,        GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_SetBits(CH582_12V_Pin); // Ϊ�͵�ƽ

    GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
    GPIOB_ResetBits(EN_TEMP_SWITCH_Pin); // Ϊ�͵�ƽ
   PRINT("����Ϊ����ģʽ: EN_TEMP_SWITCH_Pin(0x%x)=GPIO_ModeIN_PU  \n", EN_TEMP_SWITCH_Pin);
   GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin, GPIO_ModeIN_PU);
   uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 1 : 0; // ��ȡ��ǰ����״̬
   PRINT("EN_TEMP_SWITCH_Pin:%d\n",buttonLevel);
   EN_TEMP_SWITCH();
}

static uint8_t EN_TEMP_SWITCH_flag = 1; // Ĭ��high��ƽ
void EN_TEMP_SWITCH(void) {
    EN_TEMP_SWITCH_flag =!EN_TEMP_SWITCH_flag; // ��ת״̬
    if(EN_TEMP_SWITCH_flag) {
        GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
        GPIOB_SetBits(EN_TEMP_SWITCH_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(WHITE, 0.05); // ���� 5%  
        PRINT("SET 1 EN_TEMP_SWITCH_Pin:%d\n",EN_TEMP_SWITCH_Pin);
    }
    else {
        GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // ����  Ϊ�������
        GPIOB_ResetBits(EN_TEMP_SWITCH_Pin); // ����  Ϊ�͵�ƽ
        setDimColor(GREEN, 0.05); // ���� 5%  
        PRINT("reSET 0 EN_TEMP_SWITCH_Pin:%d\n",EN_TEMP_SWITCH_Pin);
    }
    PRINT("EN_TEMP_SWITCH_flag:%d\n",EN_TEMP_SWITCH_flag);
}

// ȫ�ֱ�����¼��ǰ PB5 ��״̬
static uint8_t BootState = 0; // Ĭ�ϵ͵�ƽ

void BOOT_SWICH(void) {
    // ��ת  �ĵ�ƽ
    BootState = !BootState; // ��ת״̬
    // ����  �ĵ�ƽ
    if (BootState) {
        GPIOB_SetBits(EN_CH_Pin); // �ߵ�ƽ�ر�
        GPIOB_ResetBits(CH582_12V_Pin); // ����  Ϊ�͵�ƽ
        GPIOB_ResetBits(CH582_3V3_Pin); // ����  Ϊ�͵�ƽ
        DelayMs(200); // �ӳ� 200ms
        //  Ϊ�ߵ�ƽ��������ʾΪ��ɫ
        GPIOB_SetBits(CH582_3V3_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(RED_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
        GPIOB_ResetBits(EN_CH_Pin); // �͵�ƽ��
        PRINT("3V3 EN\n");

        // �ӳ�50ms
        DelayMs(50); // �ӳ� 50ms
    } else {
        //  Ϊ�͵�ƽ��������ʾΪ��ɫ
        GPIOB_SetBits(EN_CH_Pin); // �ߵ�ƽ�ر�
        GPIOB_ResetBits(CH582_12V_Pin); // ����  Ϊ�͵�ƽ
        GPIOB_ResetBits(CH582_3V3_Pin); // ����  Ϊ�͵�ƽ
        //setDimColor(GREEN_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
        DelayMs(200); // �ӳ� 200ms
        GPIOB_SetBits(CH582_12V_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(BLUE, 0.1); // ���� WS2812 Ϊ��ɫ������ 5%
        GPIOB_ResetBits(EN_CH_Pin); // �͵�ƽ��
        PRINT("12V EN\n");
        
        // �ӳ�50ms
        DelayMs(50); // �ӳ� 50ms
    }
    
}


static uint8_t EN_CH_flag = 1; // Ĭ��high��ƽ
void EN_CH_SWITCH(void) {
    if(EN_CH_flag)
    {
        GPIOB_SetBits(EN_CH_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(WHITE, 0.05); // ���� 5%
        
    }else
    {
        GPIOB_ResetBits(EN_CH_Pin); // ����  Ϊ�͵�ƽ
        setDimColor(GREEN, 0.05); // ���� 5% 
    }
    EN_CH_flag =!EN_CH_flag; // ��ת״̬ 
    PRINT("EN_CH_flag:%d\n",EN_CH_flag);
}


static uint8_t EN_ESP_flag = 1; // Ĭ��high��ƽ
void EN_ESP_SWITCH(void) {
    EN_ESP_flag =!EN_ESP_flag; // ��ת״̬
    if(EN_ESP_flag) 
    {
        GPIOB_SetBits(EN_ESP_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(WHITE, 0.05); // ���� 5% 
    }else
    {
        GPIOB_ResetBits(EN_ESP_Pin); // ����  Ϊ�͵�ƽ
        setDimColor(GREEN, 0.05); // ���� 5%
    }
    PRINT("EN_ESP_flag:%d\n",EN_ESP_flag);
}


static uint8_t EN_ESP_ME_flag = 1; // Ĭ��high��ƽ
void EN_ESP_ME_SWITCH(void) {
    EN_ESP_ME_flag =!EN_ESP_ME_flag; // ��ת״̬
    if(EN_ESP_ME_flag) {
        GPIOA_SetBits(EN_ESP_ME_Pin); // ����  Ϊ�ߵ�ƽ
        setDimColor(WHITE, 0.05); // ���� 5%  
    } 
    else {
        GPIOA_ResetBits(EN_ESP_ME_Pin); // ����  Ϊ�͵�ƽ
        setDimColor(GREEN, 0.05); // ���� 5% 
    }
    PRINT("EN_ESP_ME_flag:%d\n",EN_ESP_ME_flag);
}

static uint8_t EN_ESP_UART1_LOG_flag = 1; // Ĭ��high��ƽ
void EN_ESP_UART1_LOG_SWITCH(void) {
    EN_ESP_UART1_LOG_flag =!EN_ESP_UART1_LOG_flag; // ��ת״̬
    if(EN_ESP_UART1_LOG_flag) {
        GPIOB_SetBits(EN_ESP_UART1_LOG_Pin); // ���� PB14 Ϊ�ߵ�ƽ
        setDimColor(WHITE, 0.05); // ���� 5%  
    }
    else {  
        GPIOB_ResetBits(EN_ESP_UART1_LOG_Pin); // ���� PB14 Ϊ�͵�ƽ
        setDimColor(GREEN, 0.05); // ���� 5%
    }
    PRINT("EN_ESP_UART1_LOG_flag:%d\n",EN_ESP_UART1_LOG_flag);
}




/**
 * @brief GPIO_B�жϷ�����
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOB_IRQHandler(void)
{
    if(R16_PB_INT_IF & CH582_PROG_BOOT_Pin) // ���PB3�жϱ�־
    {
        // ����жϱ�־
        R16_PB_INT_IF |= CH582_PROG_BOOT_Pin;  // д1��0
        // �л���������
        if(currentTriggerMode == GPIO_ITMode_FallEdge)
        {
            GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_RiseEdge);
            currentTriggerMode = GPIO_ITMode_RiseEdge;
        }
        else
        {
            GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
            currentTriggerMode = GPIO_ITMode_FallEdge;
        }
        // �������¼�
        ButtonHandler();
    }
}



/**
 * @brief TMOS���������¼�
 * 
 * @param taskId ����ID
 * @param events �¼���־
 * @return uint16_t δ������¼�
 */
//����Ϊflash����

uint16_t Key_ProcessEvent(uint8_t taskId, uint16_t events)
{
    // �������¼�
    if(events & KEY_EVENT_SINGLE_CLICK)
    {
        PRINT("���������¼�\n");
        //��תPB5 ��ƽ���ߵ�ƽʱ��������ʾΪ��ɫ���͵�ƽ����ws2812��ʾ��ɫ����ʾ����0.05
        BOOT_SWICH();

        return (events ^ KEY_EVENT_SINGLE_CLICK);
    }
    
    // ����˫���¼�
    if(events & KEY_EVENT_DOUBLE_CLICK)
    {
        PRINT("����˫���¼�\n");


        /*
        GPIOB_ResetBits(CH582_3V3_Pin); // ���� PB5 Ϊ�͵�ƽ
        setDimColor(GREEN_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
        //�ӳ�50ms
        DelayMs(50); // �ӳ� 50ms
        GPIOB_SetBits(CH582_3V3_Pin); // ���� PB5 Ϊ�ߵ�ƽ
        setDimColor(RED_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
*/


        GPIOB_ResetBits(CH582_12V_Pin); // ���� PB14 Ϊ�͵�ƽ
        DelayMs(100); // �ӳ� 50ms

        GPIOB_ResetBits(CH582_3V3_Pin); // ���� PB5 Ϊ�͵�ƽ
        setDimColor(GREEN_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
        //�ӳ�50ms
        DelayMs(50); // �ӳ� 50ms
        GPIOB_SetBits(CH582_3V3_Pin); // ���� PB5 Ϊ�ߵ�ƽ
        setDimColor(RED_COLOR, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%


        return (events ^ KEY_EVENT_DOUBLE_CLICK);
    }
    
    // �������¼�
    if(events & KEY_EVENT_LONG_PRESS)
    {
        PRINT("���������¼�\n");
        GPIOB_ResetBits(CH582_3V3_Pin); // ���� PB5 Ϊ�͵�ƽ
        GPIOB_ResetBits(CH582_12V_Pin); // ���� PB14 Ϊ�͵�ƽ
        setDimColor(BLACK, 0.05); // ���� WS2812 Ϊ��ɫ������ 5%
        return (events ^ KEY_EVENT_LONG_PRESS);
    }
    
    // ˫������¼�
    if(events & KEY_DOUBLE_CLICK_CHECK)
    {
        // ˫����ʱ���
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; //  // ��ȡPB3״̬��0=�ߵ�ƽδ���£�1=�͵�ƽ���£�
        
        if(buttonState == BUTTON_DOUBLE_CLICK_WAIT)
        {
            if(buttonLevel == 0) // ����δ���£�ȷ��Ϊ����
            {
                
                tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                buttonState = BUTTON_IDLE;
            }
            else // �����Ѱ��£�����
            {
                PRINT("Error in KEY_DOUBLE_CLICK_CHECK: �����Ѱ��µ��ж�δ����\n");
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            PRINT("Error in KEY_DOUBLE_CLICK_CHECK: �ǵȴ�˫��״̬ %d\n", buttonState);
        }
        
        return (events ^ KEY_DOUBLE_CLICK_CHECK);
    }
    
    // ��������¼�
    if(events & KEY_LONG_PRESSED_CHECK)
    {
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; // ��ȡ��ǰ����״̬
        
        if(buttonState == BUTTON_PRESSED)
        {
            if(buttonLevel == 1) // �����Դ��ڰ���״̬
            {
                // ���������¼�
                tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
                
                // ����Ϊ�½��ش�����׼����һ�ΰ���
                GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
                currentTriggerMode = GPIO_ITMode_FallEdge;
                
                // �ָ�����״̬
                buttonState = BUTTON_IDLE;
            }
            else // �������ͷŵ�û�д����ж�
            {
                PRINT("Error in KEY_LONG_PRESSED_CHECK: �������ͷŵ��ж�δ����\n");
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            PRINT("Error in KEY_LONG_PRESSED_CHECK: �ǰ���״̬ %d\n", buttonState);
        }
        
        return (events ^ KEY_LONG_PRESSED_CHECK);
    }
    
    // �����¼�����ѡ��
    if(events & KEY_TEST_SECOND)
    {
        uint16_t t32k = R16_RTC_CNT_32K;
        uint16_t sec2 = R16_RTC_CNT_2S;
        uint16_t day = R32_RTC_CNT_DAY & 0x3FFF;
        
        uint8_t i = (t32k > 16384) ? 1 : 0;
        PRINT("ʱ��: %d�� %d�� ״̬: %d\n", day, sec2*2+i, buttonState);
        
        tmos_start_task(keyTaskId, KEY_TEST_SECOND, 1600); // 1s ��ʱ
        return (events ^ KEY_TEST_SECOND);
    }
    if(events & KEY_NOISE_PRESSED)
    { 
        PRINT("�������������Դ˴��¼�\n");
        return (events ^ KEY_NOISE_PRESSED);
    }

    if(events & KEY_STATE_UNKOWN)
    { 
        PRINT("����״̬�쳣���ص�����̬\n");
        return (events ^ KEY_STATE_UNKOWN);
    }

    if(events & BUTTON_PRESSED_OVERTIME_ERR)
    { 
        PRINT("���� �ȵ����� ���ǵ������¼���������ʱ�� û�˴������ Ӧ���ǳ����¼��������������ˣ�����ȡ��������������ֱ����������¼������ص���ʼ̬\n");

        print_timestamp(&buttonPressTime);
        print_timestamp(&buttonReleaseTime);
        PRINT("pressDuration=%d \n",pressDuration);
        return (events ^ BUTTON_PRESSED_OVERTIME_ERR);
    }
    if(events & DOUBULE_PRESSED_OVERTIME_ERR)
    { 
        PRINT("˫���ĵڶ��ΰ��³�����˫���ȴ�ʱ��ϵͳû�д�������ӦΪ����һ�ε���\n");
        return (events ^ DOUBULE_PRESSED_OVERTIME_ERR);
    }



    // ����δ������¼�
    return 0;
}
