#include "CONFIG.h"
#include "key.h"
#include "ws2812.h"
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
    uint8_t buttonPressed = (R32_PB_PIN & GPIO_Pin_3) ? 0 : 1;  // 读取PB3状态（0=高电平未按下，1=低电平按下）
    
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
    
    // 配置PB3为输入模式，使能上拉
    GPIOB_ModeCfg(GPIO_Pin_3, GPIO_ModeIN_PU);
    
    // 配置PB3的中断，初始设置为下降沿触发
    GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_FallEdge);
    //GPIOB_ITCmd(GPIO_Pin_3, ENABLE); cfg已经启用
    
    // 设置默认触发模式为下降沿
    currentTriggerMode = GPIO_ITMode_FallEdge;
    
    // 启用GPIO中断
    PFIC_EnableIRQ(GPIO_B_IRQn);
    
    // 设置初始状态
    buttonState = BUTTON_IDLE;
}

// 全局变量记录当前 PB5 的状态
static uint8_t pb5State = 0; // 默认低电平
static uint8_t pb14State = 0; // 默认低电平

void CHANGE_PB5(void) {
    // 反转 PB5 的电平
    pb5State = !pb5State; // 反转状态

    // 设置 PB5 的电平
    if (pb5State) {

        GPIOB_ResetBits(GPIO_Pin_14); // 设置 PB14 为低电平
        DelayMs(200); // 延迟 200ms
        // PB5 为高电平，设置显示为红色
        GPIOB_SetBits(GPIO_Pin_5); // 设置 PB5 为高电平
        setDimColor(RED_COLOR, 0.05); // 设置 WS2812 为红色，亮度 5%
        // 延迟50ms
        DelayMs(50); // 延迟 50ms

    } else {
        // PB5 为低电平，设置显示为绿色
        GPIOB_ResetBits(GPIO_Pin_5); // 设置 PB5 为低电平
        //setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        DelayMs(200); // 延迟 200ms
        GPIOB_SetBits(GPIO_Pin_14); // 设置 PB14 为高电平
        setDimColor(BLUE, 0.1); // 设置 WS2812 为红色，亮度 5%
    }
}


void CHANGE_PB14(void) {
    // 反转 PB14 的电平
    pb14State = !pb14State; // 反转状态

    // 设置 PB5 的电平
    if (pb14State) {
        // PB5 为高电平，设置显示为红色
        GPIOB_SetBits(GPIO_Pin_14); // 设置 PB14 为高电平
        setDimColor(Purple, 0.05); // 设置 WS2812 为红色，亮度 5%
    } else {
        // PB5 为低电平，设置显示为绿色
        GPIOB_ResetBits(GPIO_Pin_14); // 设置 PB14 为低电平
        setDimColor(RED, 0.05); // 设置 WS2812 为绿色，亮度 5%
    }
}

/**
 * @brief GPIO_B中断服务函数
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOB_IRQHandler(void)
{
    if(R16_PB_INT_IF & GPIO_Pin_3) // 检查PB3中断标志
    {
        // 清除中断标志
        R16_PB_INT_IF |= GPIO_Pin_3;  // 写1清0
        // 切换触发边沿
        if(currentTriggerMode == GPIO_ITMode_FallEdge)
        {
            GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_RiseEdge);
            currentTriggerMode = GPIO_ITMode_RiseEdge;
        }
        else
        {
            GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_FallEdge);
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
uint16_t Key_ProcessEvent(uint8_t taskId, uint16_t events)
{
    // 处理单击事件
    if(events & KEY_EVENT_SINGLE_CLICK)
    {
        PRINT("按键单击事件\n");
        //反转PB5 电平，高电平时候设置显示为红色，低电平设置ws2812显示绿色，显示亮度0.05
        CHANGE_PB5();

        return (events ^ KEY_EVENT_SINGLE_CLICK);
    }
    
    // 处理双击事件
    if(events & KEY_EVENT_DOUBLE_CLICK)
    {
        PRINT("按键双击事件\n");


        /*
        GPIOB_ResetBits(GPIO_Pin_5); // 设置 PB5 为低电平
        setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        //延迟50ms
        DelayMs(50); // 延迟 50ms
        GPIOB_SetBits(GPIO_Pin_5); // 设置 PB5 为高电平
        setDimColor(RED_COLOR, 0.05); // 设置 WS2812 为红色，亮度 5%
*/


        GPIOB_ResetBits(GPIO_Pin_14); // 设置 PB14 为低电平
        DelayMs(100); // 延迟 50ms

        GPIOB_ResetBits(GPIO_Pin_5); // 设置 PB5 为低电平
        setDimColor(GREEN_COLOR, 0.05); // 设置 WS2812 为绿色，亮度 5%
        //延迟50ms
        DelayMs(50); // 延迟 50ms
        GPIOB_SetBits(GPIO_Pin_5); // 设置 PB5 为高电平
        setDimColor(RED_COLOR, 0.05); // 设置 WS2812 为红色，亮度 5%


        return (events ^ KEY_EVENT_DOUBLE_CLICK);
    }
    
    // 处理长按事件
    if(events & KEY_EVENT_LONG_PRESS)
    {
        PRINT("按键长按事件\n");
        GPIOB_ResetBits(GPIO_Pin_5); // 设置 PB5 为低电平
        GPIOB_ResetBits(GPIO_Pin_14); // 设置 PB14 为低电平
        setDimColor(WHITE, 0.05); // 设置 WS2812 为红色，亮度 5%
        return (events ^ KEY_EVENT_LONG_PRESS);
    }
    
    // 双击检测事件
    if(events & KEY_DOUBLE_CLICK_CHECK)
    {
        // 双击超时检测
        uint8_t buttonLevel = (R32_PB_PIN & GPIO_Pin_3) ? 0 : 1; //  // 读取PB3状态（0=高电平未按下，1=低电平按下）
        
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
        uint8_t buttonLevel = (R32_PB_PIN & GPIO_Pin_3) ? 0 : 1; // 读取当前按键状态
        
        if(buttonState == BUTTON_PRESSED)
        {
            if(buttonLevel == 1) // 按键仍处于按下状态
            {
                // 触发长按事件
                tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
                
                // 设置为下降沿触发，准备下一次按下
                GPIOB_ITModeCfg(GPIO_Pin_3, GPIO_ITMode_FallEdge);
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
