#include "CONFIG.h"
#include "key.h"
#include "ws2812.h"
#include "central.h"  
// 注意：需要确保包含以下头文件，它包含CH582M定时器相关定义
// #include "CH58x_common.h"  

#define SINGLE_CLICK_TIME    300  // 单击最大时间间隔（ms）
#define DOUBLE_CLICK_TIME    600  // 双击最大时间间隔（ms）
#define LONG_PRESS_TIME      1500 // 长按时间（ms）



typedef enum {
    BUTTON_IDLE,              // 空闲态
    BUTTON_PRESSED,           // 按下态
    BUTTON_DOUBLE_CLICK_WAIT  // 等待双击态
} ButtonState;

volatile ButtonState buttonState = BUTTON_IDLE;  // 按钮状态机
volatile KeyEvent keyEvent = KEY_EVENT_NONE;     // 当前按键事件
 uint8_t keyTaskId = 0xFF;                 // TMOS任务ID，用于发送按键事件

// 时间戳结构体（使用RTC寄存器）
typedef struct {
    uint16_t day;     // 天数累计（0~65535天）
    uint16_t sec2;    // 半秒计数
    uint16_t t32k;    // 32KHz时钟计数
} TimeStamp32;

// 声明全局时间变量
volatile TimeStamp32 buttonPressTime = {0};    // 按键按下时间戳
volatile TimeStamp32 buttonReleaseTime = {0};  // 按键释放时间戳
volatile TimeStamp32 currentTime = {0};        // 当前系统时间戳

// 全局变量记录当前触发模式
static uint8_t currentTriggerMode = GPIO_ITMode_FallEdge;

/**
 * @brief 获取当前RTC时间戳
 * 
 * @param timestamp 时间戳结构体指针
 */
void GetTimeStamp(volatile TimeStamp32 *timestamp)
{
    timestamp->day = R32_RTC_CNT_DAY & 0x3FFF;
    timestamp->sec2 = R16_RTC_CNT_2S;
    timestamp->t32k = R16_RTC_CNT_32K;
}

/**
 * @brief 计算两个时间戳的差值（单位：毫秒）
 * 
 * @param new 新时间戳
 * @param old 旧时间戳
 * @return int32_t 时间差（毫秒）
 */
int32_t TimeDiff(volatile TimeStamp32 *new, volatile TimeStamp32 *old)
{
    int32_t diff = 0;
    diff += (new->day - old->day) * 86400000L;    // 天转毫秒
    diff += (new->sec2 - old->sec2) * 2000;       // 2秒转毫秒
    diff += (new->t32k - old->t32k) * 30/1000;         // 32K计数转毫秒（约30.5us每个计数）
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
 * @brief 按键中断处理函数
 * 
 * @note 此函数在PB3引脚状态变化时被调用
 */
void ButtonHandler(void) 
{
    uint32_t buttonPressed = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1;  // 读取PB3状态（0=高电平未按下，1=低电平按下）
    
    //PRINT("ButtonHandler(), state=%d, pressed=%d\n", buttonState, buttonPressed);

    // 根据按键状态和触发方式处理
   
    if((buttonState == BUTTON_IDLE)&&(buttonPressed == 1)) // 空闲态下按下
    {
        GetTimeStamp(&buttonPressTime);
        buttonState = BUTTON_PRESSED;
        // 启动长按检测定时器(1.5s)
        //tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, LONG_PRESS_TIME*1600/1000);
        tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, 2400);
    }
    else if((buttonState == BUTTON_PRESSED)&&(buttonPressed == 0)) // 按下态等释放
    {
        // 取消长按检测事件
        tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);

        GetTimeStamp(&buttonReleaseTime);
        pressDuration = TimeDiff(&buttonReleaseTime, &buttonPressTime);
        
        if(pressDuration < KEY_DEBOUNCE_TIME) // 按键抖动，忽略
        {
            tmos_set_event(keyTaskId, KEY_NOISE_PRESSED);
            buttonState = BUTTON_IDLE;
        }
        else if(pressDuration < SINGLE_CLICK_TIME) // 可能是单击或双击
        {
            // 进入等待双击态
            buttonState = BUTTON_DOUBLE_CLICK_WAIT;
            // 启动双击检测事件 (600ms超时)
            //tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, DOUBLE_CLICK_TIME*1600/1000); // 600ms ÷ 0.625ms = 960
            tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, 960); // 600ms ÷ 0.625ms = 960
        
        }
        else if(pressDuration < LONG_PRESS_TIME) // 单击
        {
            tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
            buttonState = BUTTON_IDLE;
        }
        else // 长按已由定时器处理
        {
            
            tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);
            tmos_set_event(keyTaskId, BUTTON_PRESSED_OVERTIME_ERR);
            tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
            buttonState = BUTTON_IDLE;
        }
    }
    else if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 1)) // 等待双击态下又按下
    {
        // 取消双击检测事件
        tmos_stop_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK);

        GetTimeStamp(&currentTime);
        int32_t timeSinceRelease = TimeDiff(&currentTime, &buttonReleaseTime);
        
        if(timeSinceRelease < DOUBLE_CLICK_TIME) // 双击成立
        {
            // 触发双击事件
            tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK);
            // 回到空闲态
            buttonState = BUTTON_IDLE;
        }
        else // 超时，报错 理应在双击复测事件双击处理成单击
        {
            tmos_set_event(keyTaskId, DOUBULE_PRESSED_OVERTIME_ERR);
            buttonState = BUTTON_IDLE;
        }
    }
    else
    {
        unkonw_sate=KEY_STATE_UNKOWN;
        if((buttonState == BUTTON_IDLE)&&(buttonPressed == 0))//空闲态 按钮释放进入，长按超时弹起
            unkonw_sate= SINGLE_PRESSED_RELASE;
        if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 0))//双击后释放按钮
            unkonw_sate= DOUBULE_PRESSED_RELASE;
        tmos_set_event(keyTaskId, unkonw_sate);
        buttonState = BUTTON_IDLE;
    }
        
}

void GPIOB_ITCmd(uint8_t pin, FunctionalState state) {
    if (state == ENABLE) {
        // 启用中断
        R16_PB_INT_EN |= pin; // 假设这是启用中断的寄存器
    } else {
        // 禁用中断
        R16_PB_INT_EN &= ~pin; // 假设这是禁用中断的寄存器
    }
}
/**
 * @brief 初始化按键
 */
void Key_Init(void)
{
    // 注册按键事件处理函数
    keyTaskId = TMOS_ProcessEventRegister(Key_ProcessEvent);
    PRINT("按键初始化: keyTaskId=%d\n", keyTaskId);
    
    //设置为输入模式
    PRINT("设置为输入模式: CH582_Key_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_Key_Pin);
    GPIOB_ModeCfg(CH582_Key_Pin, GPIO_ModeIN_PU);

    //设置为输入模式
    PRINT("设置为输入模式: CH582_AutoCheck_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_AutoCheck_Pin);
    GPIOB_ModeCfg(CH582_AutoCheck_Pin, GPIO_ModeIN_PU);

    // 配置为输入模式，使能上拉 PB0-15才有中断
    PRINT("设置为输入模式: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_PROG_BOOT_Pin);
    GPIOB_ModeCfg(CH582_PROG_BOOT_Pin, GPIO_ModeIN_PU);
    
    //RB_PIN_INTX：由于INT24/INT25 功能引脚映射选择位 默认是0,但是1才能：INT24_/25_映射到 PB[22]/PB[23]；
    //中断对应引脚重映射 设置(R16_PIN_ALTERNATE的RB_PIN_INTX位为1
    //PRINT("设置为输入模式: RB_PIN_INTX(0x%x)=1  \n", RB_PIN_INTX);
    //R16_PIN_ALTERNATE |= RB_PIN_INTX;

    // 配置中断，初始设置为下降沿触发
    PRINT("初始设置为下降沿触发: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ITMode_FallEdge  \n", CH582_PROG_BOOT_Pin);
    GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
    //GPIOB_ITCmd(CH582_PROG_BOOT_Pin, ENABLE); cfg已经启用
    // 设置默认触发模式为下降沿
    currentTriggerMode = GPIO_ITMode_FallEdge;
    // 启用GPIO中断
    PFIC_EnableIRQ(GPIO_B_IRQn);
    // 设置初始状态
    buttonState = BUTTON_IDLE;
    CH340_CTRL_PIN_INI();
}




void CH340_CTRL_PIN_INI(void)
{
    GPIOB_ModeCfg(EN_CH_Pin, GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_SetBits(EN_CH_Pin); // 默认高电平
    GPIOB_ModeCfg(EN_ESP_Pin, GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_SetBits(EN_ESP_Pin); // 默认高电平
    GPIOA_ModeCfg(EN_ESP_ME_Pin, GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOA_SetBits(EN_ESP_ME_Pin); // 默认高电平
    GPIOB_ModeCfg(EN_ESP_UART1_LOG_Pin, GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_SetBits(EN_ESP_UART1_LOG_Pin); // 默认高电平

    GPIOB_ModeCfg(CH582_3V3_Pin, GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_ResetBits(CH582_3V3_Pin); // 为低电平
    
    GPIOB_ModeCfg(CH582_12V_Pin,        GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_SetBits(CH582_12V_Pin); // 为低电平

    GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
    GPIOB_ResetBits(EN_TEMP_SWITCH_Pin); // 为低电平
   PRINT("设置为输入模式: EN_TEMP_SWITCH_Pin(0x%x)=GPIO_ModeIN_PU  \n", EN_TEMP_SWITCH_Pin);
   GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin, GPIO_ModeIN_PU);
   uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 1 : 0; // 读取当前按键状态
   PRINT("EN_TEMP_SWITCH_Pin:%d\n",buttonLevel);
   EN_TEMP_SWITCH();
}

static uint8_t EN_TEMP_SWITCH_flag = 1; // 默认high电平
void EN_TEMP_SWITCH(void) {
    EN_TEMP_SWITCH_flag =!EN_TEMP_SWITCH_flag; // 反转状态
    if(EN_TEMP_SWITCH_flag) {
        GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
        GPIOB_SetBits(EN_TEMP_SWITCH_Pin); // 设置  为高电平
        setDimColor(WHITE, 0.05); // 亮度 5%  
        PRINT("SET 1 EN_TEMP_SWITCH_Pin:%d\n",EN_TEMP_SWITCH_Pin);
    }
    else {
        GPIOB_ModeCfg(EN_TEMP_SWITCH_Pin,   GPIO_ModeOut_PP_5mA); // 设置  为推挽输出
        GPIOB_ResetBits(EN_TEMP_SWITCH_Pin); // 设置  为低电平
        setDimColor(GREEN, 0.05); // 亮度 5%  
        PRINT("reSET 0 EN_TEMP_SWITCH_Pin:%d\n",EN_TEMP_SWITCH_Pin);
    }
    PRINT("EN_TEMP_SWITCH_flag:%d\n",EN_TEMP_SWITCH_flag);
}

// 全局变量记录当前 PB5 的状态
static uint8_t BootState = 0; // 默认低电平


static BOOL sann_State = FALSE; // 默认扫描=0
void sann_change(void)
{
    if(sann_State==FALSE)
    {
        PRINT("Disconnect BLE and stop auto reconnect\n");
        Central_DisconnectAndStopAutoReconnect();
        setDimColor(RED_COLOR, 0.1); // 设置LED为红色表示断开状态
    }else 
    {
        PRINT("Start auto reconnect\n");
        setDimColor(BLUE, 0.1); // 设置LED为蓝色表示搜索状态
        Central_StartAutoReconnect();
    }
    sann_State=!sann_State;

}

void BOOT_SWICH(void) {
    // 反转  的电平
    BootState = !BootState; // 反转状态
    // 设置  的电平
    if (BootState) {
        GPIOB_SetBits(EN_CH_Pin); // 高电平关闭
        GPIOB_ResetBits(CH582_12V_Pin); // 设置  为低电平
        GPIOB_ResetBits(CH582_3V3_Pin); // 设置  为低电平
        DelayMs(200); // 延迟 200ms
        //  为高电平，设置显示为红色
        GPIOB_SetBits(CH582_3V3_Pin); // 设置  为高电平
        setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        GPIOB_ResetBits(EN_CH_Pin); // 低电平打开
        PRINT("3V3 EN\n");

        // 延迟50ms
        DelayMs(50); // 延迟 50ms
    } else {
        //  为低电平，设置显示为绿色
        GPIOB_SetBits(EN_CH_Pin); // 高电平关闭
        GPIOB_ResetBits(CH582_12V_Pin); // 设置  为低电平
        GPIOB_ResetBits(CH582_3V3_Pin); // 设置  为低电平
        //setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        DelayMs(200); // 延迟 200ms
        GPIOB_SetBits(CH582_12V_Pin); // 设置  为高电平
        setDimColor(BLUE, 0.1); // 设置 WS2812 为红色，亮度 5%
        GPIOB_ResetBits(EN_CH_Pin); // 低电平打开
        PRINT("12V EN\n");
        
        // 延迟50ms
        DelayMs(50); // 延迟 50ms
    }
    
}


static uint8_t EN_CH_flag = 1; // 默认high电平
void EN_CH_SWITCH(void) {
    if(EN_CH_flag)
    {
        GPIOB_SetBits(EN_CH_Pin); // 设置  为高电平
        setDimColor(WHITE, 0.05); // 亮度 5%
        
    }else
    {
        GPIOB_ResetBits(EN_CH_Pin); // 设置  为低电平
        setDimColor(GREEN, 0.05); // 亮度 5% 
    }
    EN_CH_flag =!EN_CH_flag; // 反转状态 
    PRINT("EN_CH_flag:%d\n",EN_CH_flag);
}


static uint8_t EN_ESP_flag = 1; // 默认high电平
void EN_ESP_SWITCH(void) {
    EN_ESP_flag =!EN_ESP_flag; // 反转状态
    if(EN_ESP_flag) 
    {
        GPIOB_SetBits(EN_ESP_Pin); // 设置  为高电平
        setDimColor(WHITE, 0.05); // 亮度 5% 
    }else
    {
        GPIOB_ResetBits(EN_ESP_Pin); // 设置  为低电平
        setDimColor(GREEN, 0.05); // 亮度 5%
    }
    PRINT("EN_ESP_flag:%d\n",EN_ESP_flag);
}


static uint8_t EN_ESP_ME_flag = 1; // 默认high电平
void EN_ESP_ME_SWITCH(void) {
    EN_ESP_ME_flag =!EN_ESP_ME_flag; // 反转状态
    if(EN_ESP_ME_flag) {
        GPIOA_SetBits(EN_ESP_ME_Pin); // 设置  为高电平
        setDimColor(WHITE, 0.05); // 亮度 5%  
    } 
    else {
        GPIOA_ResetBits(EN_ESP_ME_Pin); // 设置  为低电平
        setDimColor(GREEN, 0.05); // 亮度 5% 
    }
    PRINT("EN_ESP_ME_flag:%d\n",EN_ESP_ME_flag);
}

static uint8_t EN_ESP_UART1_LOG_flag = 1; // 默认high电平
void EN_ESP_UART1_LOG_SWITCH(void) {
    EN_ESP_UART1_LOG_flag =!EN_ESP_UART1_LOG_flag; // 反转状态
    if(EN_ESP_UART1_LOG_flag) {
        GPIOB_SetBits(EN_ESP_UART1_LOG_Pin); // 设置 PB14 为高电平
        setDimColor(WHITE, 0.05); // 亮度 5%  
    }
    else {  
        GPIOB_ResetBits(EN_ESP_UART1_LOG_Pin); // 设置 PB14 为低电平
        setDimColor(GREEN, 0.05); // 亮度 5%
    }
    PRINT("EN_ESP_UART1_LOG_flag:%d\n",EN_ESP_UART1_LOG_flag);
}




/**
 * @brief GPIO_B中断服务函数
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOB_IRQHandler(void)
{
    if(R16_PB_INT_IF & CH582_PROG_BOOT_Pin) // 检查PB3中断标志
    {
        // 清除中断标志
        R16_PB_INT_IF |= CH582_PROG_BOOT_Pin;  // 写1清0
        // 切换触发边沿
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
        // 处理按键事件
        ButtonHandler();
    }
}



/**
 * @brief TMOS任务处理按键事件
 * 
 * @param taskId 任务ID
 * @param events 事件标志
 * @return uint16_t 未处理的事件
 */
//设置为flash区域

uint16_t Key_ProcessEvent(uint8_t taskId, uint16_t events)
{
    // 处理单击事件
    if(events & KEY_EVENT_SINGLE_CLICK)
    {
        PRINT("按键单击事件\n");
        //反转PB5 电平，高电平时候设置显示为红色，低电平设置ws2812显示绿色，显示亮度0.05
        BOOT_SWICH();

        return (events ^ KEY_EVENT_SINGLE_CLICK);
    }
    
    // 处理双击事件
    if(events & KEY_EVENT_DOUBLE_CLICK)
    {
        PRINT("按键双击事件\n");
/*

        GPIOB_ResetBits(CH582_12V_Pin); // 设置 PB14 为低电平
        DelayMs(100); // 延迟 50ms

        GPIOB_ResetBits(CH582_3V3_Pin); // 设置 PB5 为低电平
        setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        //延迟50ms
        DelayMs(50); // 延迟 50ms
        GPIOB_SetBits(CH582_3V3_Pin); // 设置 PB5 为高电平
        setDimColor(RED_COLOR, 0.05); // 设置 WS2812 为红色，亮度 5%
*/

        tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 10); // 10ms后开始发送
        return (events ^ KEY_EVENT_DOUBLE_CLICK);
    }
    
    // 处理长按事件
    if(events & KEY_EVENT_LONG_PRESS)
    {
        PRINT("按键长按事件\n");
        
        sann_change();
        /*
        // 检查当前BLE连接状态并执行相应操作
        if(Central_IsConnected())
        {
            PRINT("长按：当前有BLE连接，断开连接并停止自动重连\n");
            Central_DisconnectAndStopAutoReconnect();
            setDimColor(RED_COLOR, 0.1); // 设置LED为红色表示断开状态
        }
        else
        {
            PRINT("长按：当前无BLE连接，启动自动搜索和连接\n");
            Central_StartAutoReconnect();
            setDimColor(BLUE, 0.1); // 设置LED为蓝色表示搜索状态
        }*/
        
        return (events ^ KEY_EVENT_LONG_PRESS);
    }
    
    // 双击检测事件
    if(events & KEY_DOUBLE_CLICK_CHECK)
    {
        // 双击超时检测
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; //  // 读取PB3状态（0=高电平未按下，1=低电平按下）
        
        if(buttonState == BUTTON_DOUBLE_CLICK_WAIT)
        {
            if(buttonLevel == 0) // 按键未按下，确认为单击
            {
                
                tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                buttonState = BUTTON_IDLE;
            }
            else // 按键已按下，报错
            {
                PRINT("Error in KEY_DOUBLE_CLICK_CHECK: 按键已按下但中断未触发\n");
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            PRINT("Error in KEY_DOUBLE_CLICK_CHECK: 非等待双击状态 %d\n", buttonState);
        }
        
        return (events ^ KEY_DOUBLE_CLICK_CHECK);
    }
    
    // 长按检测事件
    if(events & KEY_LONG_PRESSED_CHECK)
    {
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; // 读取当前按键状态
        
        if(buttonState == BUTTON_PRESSED)
        {
            if(buttonLevel == 1) // 按键仍处于按下状态
            {
                // 触发长按事件
                tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
                
                // 设置为下降沿触发，准备下一次按下
                GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
                currentTriggerMode = GPIO_ITMode_FallEdge;
                
                // 恢复空闲状态
                buttonState = BUTTON_IDLE;
            }
            else // 按键已释放但没有触发中断
            {
                PRINT("Error in KEY_LONG_PRESSED_CHECK: 按键已释放但中断未触发\n");
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            PRINT("Error in KEY_LONG_PRESSED_CHECK: 非按下状态 %d\n", buttonState);
        }
        
        return (events ^ KEY_LONG_PRESSED_CHECK);
    }
    
    // 测试事件（可选）
    if(events & KEY_TEST_SECOND)
    {
        uint16_t t32k = R16_RTC_CNT_32K;
        uint16_t sec2 = R16_RTC_CNT_2S;
        uint16_t day = R32_RTC_CNT_DAY & 0x3FFF;
        
        uint8_t i = (t32k > 16384) ? 1 : 0;
        PRINT("时间: %d天 %d秒 状态: %d\n", day, sec2*2+i, buttonState);
        
        tmos_start_task(keyTaskId, KEY_TEST_SECOND, 1600); // 1s 定时
        return (events ^ KEY_TEST_SECOND);
    }
    if(events & KEY_NOISE_PRESSED)
    { 
        PRINT("按键抖动，忽略此次事件\n");
        return (events ^ KEY_NOISE_PRESSED);
    }

    if(events & KEY_STATE_UNKOWN)
    { 
        PRINT("按键状态异常，回到空闲态\n");
        return (events ^ KEY_STATE_UNKOWN);
    }

    if(events & BUTTON_PRESSED_OVERTIME_ERR)
    { 
        PRINT("按下 等弹起来 但是弹起来事件超过长按时间 没人处理错误 应该是长按事件被其他任务耽误了，这里取消长按复测任务，直接输出长按事件，并回到初始态\n");

        print_timestamp(&buttonPressTime);
        print_timestamp(&buttonReleaseTime);
        PRINT("pressDuration=%d \n",pressDuration);
        return (events ^ BUTTON_PRESSED_OVERTIME_ERR);
    }
    if(events & DOUBULE_PRESSED_OVERTIME_ERR)
    { 
        PRINT("双击的第二次按下超过了双击等待时间系统没有处理，报错，应为另外一次单击\n");
        return (events ^ DOUBULE_PRESSED_OVERTIME_ERR);
    }



    // 返回未处理的事件
    return 0;
}
