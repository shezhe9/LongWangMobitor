/*
 * key.h - 多按键管理系统
 *
 *  Created on: Mar 18, 2025
 *      Author: Lee
 *  Modified: Oct 11, 2025 - 重构支持多按键
 */

#ifndef INCLUDE_KEY_H_
#define INCLUDE_KEY_H_
#include "CH58x_common.h"

/******************** 按键配置 ********************/

// 最大按键数量（10个GPIOA + 1个GPIOB）
#define MAX_KEYS 11

// GPIO端口标识
#define KEY_GPIO_A  0  // GPIOA端口
#define KEY_GPIO_B  1  // GPIOB端口

// 按键引脚定义（前10个在GPIOA，第11个在GPIOB）
#define KEY_PIN_0   GPIO_Pin_4   // PA4
#define KEY_PIN_1   GPIO_Pin_5   // PA5
#define KEY_PIN_2   GPIO_Pin_6   // PA6
#define KEY_PIN_3   GPIO_Pin_0   // PA0
#define KEY_PIN_4   GPIO_Pin_1   // PA1
#define KEY_PIN_5   GPIO_Pin_2   // PA2
#define KEY_PIN_6   GPIO_Pin_3   // PA3
#define KEY_PIN_7   GPIO_Pin_15  // PA15
#define KEY_PIN_8   GPIO_Pin_14  // PA14
#define KEY_PIN_9   GPIO_Pin_7   // PA7
#define KEY_PIN_10  GPIO_Pin_1   // PB1（原PROG_BOOT引脚）

// 按键使能开关（1=启用，0=禁用）
#define KEY_ENABLE_0   1  // PA4
#define KEY_ENABLE_1   1  // PA5
#define KEY_ENABLE_2   1  // PA6
#define KEY_ENABLE_3   1  // PA0
#define KEY_ENABLE_4   1  // PA1
#define KEY_ENABLE_5   1  // PA2
#define KEY_ENABLE_6   1  // PA3
#define KEY_ENABLE_7   1  // PA15
#define KEY_ENABLE_8   1  // PA14
#define KEY_ENABLE_9   1  // PA7
#define KEY_ENABLE_10  1  // PB1（原PROG_BOOT）

// 向后兼容：保留原有PB1定义
#define CH582_PROG_BOOT_Pin KEY_PIN_10  // PB1 按键引脚

/******************** 时间参数配置 ********************/

// 定义按键去抖时间（毫秒）
#define KEY_DEBOUNCE_TIME      50    // 50ms去抖时间
#define SINGLE_CLICK_TIME      300   // 单击最大时间间隔
#define DOUBLE_CLICK_TIME      600   // 双击最大时间间隔
#define LONG_PRESS_TIME        1500  // 长按时间（1.5s）

// TMOS定时器参数（单位：0.625ms tick）
#define KEY_LONG_PRESS_TICKS   2400  // 1500ms ÷ 0.625ms = 2400
#define KEY_DOUBLE_CLICK_TICKS 960   // 600ms ÷ 0.625ms = 960

/******************** 按键状态定义 ********************/

// 按键状态机
typedef enum {
    BUTTON_IDLE,              // 空闲态
    BUTTON_PRESSED,           // 按下态
    BUTTON_DOUBLE_CLICK_WAIT  // 等待双击态
} ButtonState;

// 按键事件类型
typedef enum {
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SINGLE_CLICK = 0x0001,  // 单击事件
    KEY_EVENT_DOUBLE_CLICK = 0x0002,  // 双击事件
    KEY_EVENT_LONG_PRESS   = 0x0004,  // 长按事件
    KEY_TEST_SECOND        = 0x0008   // 测试事件
} KeyEvent;

// 按键检测事件
#define KEY_DOUBLE_CLICK_CHECK     0x0010  // 双击检测事件
#define KEY_LONG_PRESSED_CHECK     0x0020  // 长按检测事件
#define KEY_NOISE_PRESSED          0x0040  // 抖动事件

// 按键错误事件
#define KEY_STATE_UNKOWN               0x0100  // 按键状态错误
#define BUTTON_PRESSED_OVERTIME_ERR    0x0200  // 按下超时错误
#define DOUBULE_PRESSED_OVERTIME_ERR   0x0400  // 双击超时错误

// 按键释放事件
#define SINGLE_PRESSED_RELASE      0x1000  // 单击释放
#define DOUBULE_PRESSED_RELASE     0x2000  // 双击释放

/******************** 时间戳结构体 ********************/

// RTC时间戳结构体（使用硬件RTC寄存器）
typedef struct {
    uint16_t day;     // 天数累计（0~65535天）
    uint16_t sec2;    // 半秒计数（每2秒+1）
    uint16_t t32k;    // 32KHz时钟计数
} TimeStamp32;

/******************** 按键对象结构体 ********************/

// 按键对象（每个按键独立管理）
typedef struct {
    uint16_t pin;                // GPIO引脚掩码
    uint8_t gpio;                // GPIO端口（0=GPIOA, 1=GPIOB）
    uint8_t enabled;             // 使能标志（1=启用，0=禁用）
    ButtonState state;           // 状态机
    uint8_t triggerMode;         // 当前触发边沿模式
    TimeStamp32 pressTime;       // 按下时间戳
    TimeStamp32 releaseTime;     // 释放时间戳
    int32_t pressDuration;       // 按键持续时间（毫秒）
} KeyObject_t;

/******************** 全局变量声明 ********************/

extern uint8_t keyTaskId;        // TMOS任务ID
extern KeyObject_t keys[MAX_KEYS]; // 按键对象数组
extern uint8_t activeKeyId;      // 当前活动的按键ID

/******************** 函数声明 ********************/

/**
 * @brief 初始化按键系统（支持多按键）
 */
void Key_Init(void);

/**
 * @brief TMOS事件处理函数
 * @param task_id 任务ID
 * @param events 事件标志
 * @return 未处理的事件
 */
uint16_t Key_ProcessEvent(uint8_t task_id, uint16_t events);

/**
 * @brief 获取RTC时间戳
 * @param timestamp 时间戳结构体指针
 */
void GetTimeStamp(volatile TimeStamp32 *timestamp);

/**
 * @brief 计算时间差（毫秒）
 * @param new 新时间戳
 * @param old 旧时间戳
 * @return 时间差（毫秒）
 */
int32_t TimeDiff(volatile TimeStamp32 *new, volatile TimeStamp32 *old);

/**
 * @brief 打印时间戳（调试用）
 * @param time 时间戳指针
 */
void print_timestamp(TimeStamp32 *time);

/**
 * @brief 获取按键事件（兼容旧版接口）
 * @return 按键事件
 */
KeyEvent Key_GetEvent(void);

/******************** 设计说明 ********************/
/*

【多按键架构设计】

1. 按键配置（编译期）
   - 通过 KEY_ENABLE_x 宏控制每个按键的启用/禁用
   - 通过 KEY_PIN_x 宏定义每个按键的GPIO引脚

2. 按键对象数组
   - KeyObject_t keys[MAX_KEYS] 管理10个按键
   - 每个按键独立的状态机、时间戳、触发模式

3. 统一中断处理
   - GPIOA_IRQHandler() 遍历检查所有启用的按键
   - 通过 activeKeyId 记录当前触发的按键ID

4. RTC硬件计时
   - 使用 R32_RTC_CNT_DAY、R16_RTC_CNT_2S、R16_RTC_CNT_32K
   - 时间差精度：~30μs（32.768KHz晶振）

5. TMOS事件处理
   - 共享事件位（KEY_EVENT_SINGLE_CLICK等）
   - 通过 activeKeyId 区分不同按键的事件

6. 双边沿切换
   - 按下：下降沿 → 上升沿（等待释放）
   - 释放：上升沿 → 下降沿（等待下次按下）

【状态机流程】
IDLE → 按下 → PRESSED → 释放(<300ms) → DOUBLE_CLICK_WAIT → 再按下(<600ms) → 双击
     ↑                ↓                    ↓
     └────────────────┴──释放(>300ms)──────┴→ 单击
                      ↓
                   长按(>1500ms)

【定时器机制】
- KEY_LONG_PRESSED_CHECK: 1.5秒后检测是否长按
- KEY_DOUBLE_CLICK_CHECK: 600ms后检测是否双击

*/

#endif /* INCLUDE_KEY_H_ */
