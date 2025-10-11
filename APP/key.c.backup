#include "CONFIG.h"
#include "key.h"
#include "central.h"
#include "ulog_buffer.h"  // ulog 日志系统  

#define SINGLE_CLICK_TIME    300  // 单击最大时间间隔（ms?
#define DOUBLE_CLICK_TIME    600  // 双击最大时间间隔（ms?
#define LONG_PRESS_TIME      1500 // 长按时间（ms?



typedef enum {
    BUTTON_IDLE,              // 空闲?
    BUTTON_PRESSED,           // 按下?
    BUTTON_DOUBLE_CLICK_WAIT  // 等待双击?
} ButtonState;

volatile ButtonState buttonState = BUTTON_IDLE;  // 按钮状态机
volatile KeyEvent keyEvent = KEY_EVENT_NONE;     // 当前按键事件
 uint8_t keyTaskId = 0xFF;                 // TMOS任务ID，用于发送按键事?

// 时间戳结构体（使用RTC寄存器）
typedef struct {
    uint16_t day;     // 天数累计?~65535天）
    uint16_t sec2;    // 半秒计数
    uint16_t t32k;    // 32KHz时钟计数
} TimeStamp32;

// 声明全局时间变量
volatile TimeStamp32 buttonPressTime = {0};    // 按键按下时间?
volatile TimeStamp32 buttonReleaseTime = {0};  // 按键释放时间?
volatile TimeStamp32 currentTime = {0};        // 当前系统时间?

// 全局变量记录当前触发模式
static uint8_t currentTriggerMode = GPIO_ITMode_FallEdge;

/**
 * @brief 获取当前RTC时间?
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
 * @return int32_t 时间差（毫秒?
 */
int32_t TimeDiff(volatile TimeStamp32 *new, volatile TimeStamp32 *old)
{
    int32_t diff = 0;
    diff += (new->day - old->day) * 86400000L;    // 天转毫秒
    diff += (new->sec2 - old->sec2) * 2000;       // 2秒转毫秒
    diff += (new->t32k - old->t32k) * 1000 / 32768;   // 32K计数转毫秒（32.768KHz）
    return diff;
}

void print_timestamp(TimeStamp32 *time)
{
    uinfo("R16_RTC_CNT_2S=%d  ",time->sec2) ;
    uinfo("32K=%d \n",time->t32k) ;
}

int32_t pressDuration=0;
uint16_t unkonw_sate=0;
/**
 * @brief 按键中断处理函数
 * 
 * @note 此函数在PB3引脚状态变化时被调?
 */
void ButtonHandler(void) 
{
    uint32_t buttonPressed = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1;  // 读取PB3状态（0=高电平未按下?=低电平按下）
    
    //uinfo("ButtonHandler(), state=%d, pressed=%d\n", buttonState, buttonPressed);

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
        // 取消长按检测事?
        tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);

        GetTimeStamp(&buttonReleaseTime);
        pressDuration = TimeDiff(&buttonReleaseTime, &buttonPressTime);
        
        if(pressDuration < KEY_DEBOUNCE_TIME) // 按键抖动，忽?
        {
            tmos_set_event(keyTaskId, KEY_NOISE_PRESSED);
            buttonState = BUTTON_IDLE;
        }
        else if(pressDuration < SINGLE_CLICK_TIME) // 可能是单击或双击
        {
            // 进入等待双击?
            buttonState = BUTTON_DOUBLE_CLICK_WAIT;
            // 启动双击检测事?(600ms超时)
            //tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, DOUBLE_CLICK_TIME*1600/1000); // 600ms ÷ 0.625ms = 960
            tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, 960); // 600ms ÷ 0.625ms = 960
        
        }
        else if(pressDuration < LONG_PRESS_TIME) // 单击
        {
            tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
            buttonState = BUTTON_IDLE;
        }
        else // 长按已由定时器处?
        {
            
            tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);
            tmos_set_event(keyTaskId, BUTTON_PRESSED_OVERTIME_ERR);
            tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
            buttonState = BUTTON_IDLE;
        }
    }
    else if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 1)) // 等待双击态下又按?
    {
        // 取消双击检测事?
        tmos_stop_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK);

        GetTimeStamp(&currentTime);
        int32_t timeSinceRelease = TimeDiff(&currentTime, &buttonReleaseTime);
        
        if(timeSinceRelease < DOUBLE_CLICK_TIME) // 双击成立
        {
            // 触发双击事件
            tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK);
            // 回到空闲?
            buttonState = BUTTON_IDLE;
        }
        else // 超时，报?理应在双击复测事件双击处理成单击
        {
            tmos_set_event(keyTaskId, DOUBULE_PRESSED_OVERTIME_ERR);
            buttonState = BUTTON_IDLE;
        }
    }
    else
    {
        unkonw_sate=KEY_STATE_UNKOWN;
        if((buttonState == BUTTON_IDLE)&&(buttonPressed == 0))//空闲?按钮释放进入，长按超时弹?
            unkonw_sate= SINGLE_PRESSED_RELASE;
        if((buttonState == BUTTON_DOUBLE_CLICK_WAIT)&&(buttonPressed == 0))//双击后释放按?
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
 * @brief 初始化按?
 */
void Key_Init(void)
{
    // 注册按键事件处理函数
    keyTaskId = TMOS_ProcessEventRegister(Key_ProcessEvent);
    uinfo("\260\264\274\374\263\365\312\274\273\257: keyTaskId=%d\n", keyTaskId);  // 按键初始?
    
    // 配置为输入模式，使能上拉 PB0-15才有中断
    uinfo("\311\350\326\303\316\252\312\344\310\353\304\243\312\275: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ModeIN_PU  \n", CH582_PROG_BOOT_Pin);  // 设置为输入模?
    GPIOB_ModeCfg(CH582_PROG_BOOT_Pin, GPIO_ModeIN_PU);
    
    // 配置中断，初始设置为下降沿触?
    uinfo("\263\365\312\274\311\350\326\303\316\252\317\302\275\265\321\330\264\245\267\242: CH582_PROG_BOOT_Pin(0x%x)=GPIO_ITMode_FallEdge  \n", CH582_PROG_BOOT_Pin);  // 初始设置为下降沿触发
    GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
    
    // 设置默认触发模式为下降沿
    currentTriggerMode = GPIO_ITMode_FallEdge;
    // 启用GPIO中断
    PFIC_EnableIRQ(GPIO_B_IRQn);
    // 设置初始状?
    buttonState = BUTTON_IDLE;
}

// 全局变量记录当前 PB5 的状?
static BOOL sann_State = FALSE; // 默认扫描=0
void sann_change(void)
{
    if(sann_State==FALSE)
    {
        uinfo("Disconnect BLE and stop auto reconnect\n");
        Central_DisconnectAndStopAutoReconnect();
    }else 
    {
        uinfo("Start auto reconnect\n");
        Central_StartAutoReconnect();
    }
    sann_State=!sann_State;

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
        R16_PB_INT_IF |= CH582_PROG_BOOT_Pin;  // ??
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
        uinfo("\260\264\274\374\265\245\273\367\312\302\274\376\n");  // 按键单击事件
        // 单击事件处理（BOOT_SWICH已删除）
        return (events ^ KEY_EVENT_SINGLE_CLICK);
    }
    
    // 处理双击事件
    if(events & KEY_EVENT_DOUBLE_CLICK)
    {
        uinfo("\260\264\274\374\313\253\273\367\312\302\274\376\n");  // 按键双击事件
/*

        GPIOB_ResetBits(CH582_12V_Pin); // 设置 PB14 为低电平
        DelayMs(100); // 延迟 50ms

        GPIOB_ResetBits(CH582_3V3_Pin); // 设置 PB5 为低电平
        //延迟50ms
        DelayMs(50); // 延迟 50ms
        GPIOB_SetBits(CH582_3V3_Pin); // 设置 PB5 为高电平
*/

        tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 10); // 10ms后开始发?
        return (events ^ KEY_EVENT_DOUBLE_CLICK);
    }
    
    // 处理长按事件
    if(events & KEY_EVENT_LONG_PRESS)
    {
        uinfo("\260\264\274\374\263\244\260\264\312\302\274\376\n");  // 按键长按事件
        
        sann_change();
        /*
        // 检查当前BLE连接状态并执行相应操作
        if(Central_IsConnected())
        {
            uinfo("\263\244\260\264:\265\261\307\260\323\320 BLE \301\254\275\323,\266\317\277\252\301\254\275\323\262\242\315\243\326\271\327\324\266\257\326\330\301\254\n");  // 长按当前有连接断开连接并停止自动重?
            Central_DisconnectAndStopAutoReconnect();
        }
        else
        {
            uinfo("\263\244\260\264:\265\261\307\260\316\336 BLE \301\254\275\323,\306\364\266\257\327\324\266\257\313\321\313\367\272\315\301\254\275\323\n");  // 长按当前无连接启动自动搜索和连接
            Central_StartAutoReconnect();
        }*/
        
        return (events ^ KEY_EVENT_LONG_PRESS);
    }
    
    // 双击检测事?
    if(events & KEY_DOUBLE_CLICK_CHECK)
    {
        // 双击超时检?
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; //  // 读取PB3状态（0=高电平未按下?=低电平按下）
        
        if(buttonState == BUTTON_DOUBLE_CLICK_WAIT)
        {
            if(buttonLevel == 0) // 按键未按下，确认为单?
            {
                
                tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                buttonState = BUTTON_IDLE;
            }
            else // 按键已按下，报错
            {
                uinfo("Error in KEY_DOUBLE_CLICK_CHECK: \260\264\274\374\322\321\260\264\317\302\265\253\326\320\266\317\316\264\264\245\267\242\n");  // 按键已按下但中断未触?
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            uinfo("Error in KEY_DOUBLE_CLICK_CHECK: \267\307\265\310\264\375\313\253\273\367\327\264\314\254 %d\n", buttonState);  // 非等待双击状?
        }
        
        return (events ^ KEY_DOUBLE_CLICK_CHECK);
    }
    
    // 长按检测事?
    if(events & KEY_LONG_PRESSED_CHECK)
    {
        uint8_t buttonLevel = (R32_PB_PIN & CH582_PROG_BOOT_Pin) ? 0 : 1; // 读取当前按键状?
        
        if(buttonState == BUTTON_PRESSED)
        {
            if(buttonLevel == 1) // 按键仍处于按下状?
            {
                // 触发长按事件
                tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
                
                // 设置为下降沿触发，准备下一次按?
                GPIOB_ITModeCfg(CH582_PROG_BOOT_Pin, GPIO_ITMode_FallEdge);
                currentTriggerMode = GPIO_ITMode_FallEdge;
                
                // 恢复空闲状?
                buttonState = BUTTON_IDLE;
            }
            else // 按键已释放但没有触发中断
            {
                uinfo("Error in KEY_LONG_PRESSED_CHECK: \260\264\274\374\322\321\312\315\267\305\265\253\326\320\266\317\316\264\264\245\267\242\n");  // 按键已释放但中断未触?
                buttonState = BUTTON_IDLE;
            }
        }
        else
        {
            uinfo("Error in KEY_LONG_PRESSED_CHECK: \267\307\260\264\317\302\327\264\314\254 %d\n", buttonState);  // 非按下状?
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
        uinfo("\312\261\274\344: %d \314\354 %d \303\353 \327\264\314\254: %d\n", day, sec2*2+i, buttonState);  // 时间天秒状?
        
        tmos_start_task(keyTaskId, KEY_TEST_SECOND, 1600); // 1s 定时
        return (events ^ KEY_TEST_SECOND);
    }
    if(events & KEY_NOISE_PRESSED)
    { 
        udebug("\260\264\274\374\266\266\266\257,\272\366\302\324\264\313\264\316\312\302\274\376\n");  // 按键抖动忽略此次事件
        return (events ^ KEY_NOISE_PRESSED);
    }

    if(events & KEY_STATE_UNKOWN)
    { 
        uwarn("\260\264\274\374\327\264\314\254\322\354\263\243,\273\330\265\275\277\325\317\320\314\254\n");  // 按键状态异常回到空闲态
        return (events ^ KEY_STATE_UNKOWN);
    }

    if(events & BUTTON_PRESSED_OVERTIME_ERR)
    { 
        uerror("\260\264\317\302 \265\310\265\257\306\360\300\264 \265\253\312\307\265\257\306\360\300\264\312\302\274\376\263\254\271\375\263\244\260\264\312\261\274\344 \303\273\310\313\264\246\300\355\264\355\316\363 \323\246\270\303\312\307\263\244\260\264\312\302\274\376\261\273\306\344\313\373\310\316\316\361\265\242\316\363\301\313,\325\342\300\357\310\241\317\373\263\244\260\264\270\264\262\342\310\316\316\361,\326\261\275\323\312\344\263\366\263\244\260\264\312\302\274\376,\262\242\273\330\265\275\263\365\312\274\314\254\n");  // 按下等弹起来但是弹起来事件超过长按时间没人处理错误应该是长按事件被其他任务耽误了这里取消长按复测任务直接输出长按事件并回到初始态

        print_timestamp(&buttonPressTime);
        print_timestamp(&buttonReleaseTime);
        uerror("pressDuration=%d \n",pressDuration);
        return (events ^ BUTTON_PRESSED_OVERTIME_ERR);
    }
    if(events & DOUBULE_PRESSED_OVERTIME_ERR)
    { 
        uwarn("\313\253\273\367\265\304\265\332\266\376\264\316\260\264\317\302\263\254\271\375\301\313\313\253\273\367\265\310\264\375\312\261\274\344\317\265\315\263\303\273\323\320\264\246\300\355,\261\250\264\355,\323\246\316\252\301\355\315\342\322\273\264\316\265\245\273\367\n");  // 双击的第二次按下超过了双击等待时间系统没有处理报错应为另外一次单击
        return (events ^ DOUBULE_PRESSED_OVERTIME_ERR);
    }



    // 返回未处理的事件
    return 0;
}

