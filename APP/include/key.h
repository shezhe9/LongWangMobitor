/*
 * key.h
 *
 *  Created on: Mar 18, 2025
 *      Author: Lee
 */

 #ifndef INCLUDE_KEY_H_
 #define INCLUDE_KEY_H_
 #include "CH58x_common.h" // 确保包含芯片头文件
 
 // 按键事件类型定义
 typedef enum {
     KEY_EVENT_NONE = 0,
     KEY_EVENT_SINGLE_CLICK = 0x0001,
     KEY_EVENT_DOUBLE_CLICK = 0x0002,
     KEY_EVENT_LONG_PRESS = 0x0004,
     KEY_TEST_SECOND = 0x0008
 } KeyEvent;
 
 extern  uint8_t keyTaskId;                 // TMOS任务ID，用于发送按键事件
 
 #define CH582_AutoCheck_Pin GPIO_Pin_18
 #define CH582_PROG_BOOT_Pin GPIO_Pin_2
 
 #define CH582_Key_Pin GPIO_Pin_19
 
 #define CH582_12V_Pin GPIO_Pin_0
 #define CH582_3V3_Pin GPIO_Pin_5
 
 #define EN_CH_Pin GPIO_Pin_6  //PB6  CH582 串口
 #define EN_ESP_Pin GPIO_Pin_12 //PB12 ESP烧录口
 #define EN_ESP_ME_Pin GPIO_Pin_12//PA12 本机ESP烧录串口
 #define EN_ESP_UART1_LOG_Pin GPIO_Pin_1//PB1  本机或者ESP 串口1
 
 #define EN_TEMP_SWITCH_Pin GPIO_Pin_9//PB9  temp切换
 
 // 按键状态定义
 
 // 新增按键检测事件
 #define KEY_DOUBLE_CLICK_CHECK  0x0010  // 双击检测事件
 #define KEY_LONG_PRESSED_CHECK  0x0020  // 长按检测事件
 #define KEY_NOISE_PRESSED       0x0040  // 抖动事件
 #define KEY_STATE_UNKOWN                    0x0100  // 按键状态错误
 #define BUTTON_PRESSED_OVERTIME_ERR         0x0200  // 按下超时过了长按时间没人处理错误
 #define DOUBULE_PRESSED_OVERTIME_ERR         0x0400  // 双击的第二次按下超过了双击等待时间系统没有处理，报错，应为另外一次单击
 
 #define SINGLE_PRESSED_RELASE           0x1000  // 单击弹起来
 #define DOUBULE_PRESSED_RELASE          0x2000  // 双击弹起来
 
 // 定义按键去抖时间（毫秒）
 #define KEY_DEBOUNCE_TIME      50   // 50ms去抖时间
 #define SINGLE_CLICK_TIME      300  // 单击最大时间间隔
 #define DOUBLE_CLICK_TIME      600  // 双击最大时间间隔
 #define LONG_PRESS_TIME        1500 // 长按时间（1.5s）
 
 // 函数声明
 void Key_Init(void);
 uint16_t Key_ProcessEvent(uint8_t task_id, uint16_t events);
 KeyEvent Key_GetEvent(void);
 
 /*
 
 帮我修改按键驱动
 按键采用中断+计时器+定时器组成
 计时器采用RTC寄存器来计算时间戳来计时，定时器采用TMOS系统的 tmos_start_task 参数部分加入定时事件
 按键驱动设计三个状态机(空闲态BUTTON_IDLE，按下态BUTTON_PRESSED，等待双击态BUTTON_DOUBLE_CLICK_WAIT)，两个定时检测事件（KEY_DOUBLE_CLICK_CHECK, KEY_LONG_PRESSED_CHECK）。
 驱动逻辑：
 初始化按钮为下降沿触发中断
 默认按钮没有按下时,为空闲态(BUTTON_IDLE)，
 按下后触发进入中断，修改中断为上升沿触发，记录时间戳1(buttonPressTime),并创建1.5s延迟检测电平任务(tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, 2400)),然后进入按下态(BUTTON_PRESSED)。
 等按钮释放后，中断响应上升沿，中断改为下降沿触发，记录时间戳2(buttonReleaseTime)，计算时间戳2-1，看时间差是否小于50ms，是的话认为当前只是毛刺，返回(BUTTON_IDLE)，取消长按检测事件tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK）。
 如果时间戳大于50且小于300ms，判定为可能单击或者双击，进入等待双击态BUTTON_DOUBLE_CLICK_WAIT,并创建双击复测事件KEY_DOUBLE_CLICK_CHECK。
 如果时间戳大于300ms且小于1.5s，判定为单击，返回休息态
 
 当按钮再次按下，中断响应下降沿触发，记录时间戳3，记算时间戳3-2是否小于600ms，是的话双击事件成立，输出双击事件，取消双击复测事件，返回空闲态：否的话报错并返回空闲态
 
 双击复测事件机制为：检测按键电平为高，说明是单击，不用在等待了，输出单击事件，状态机改回空闲态。检测电平为低报错，因为按钮如果为低会在中断进行处理。
 
 长按复测事件机制为：检测按键电平为低，说明是长按，不用等按钮释放了，改中断为下降沿触发，输出长按事件，按钮状态机回复空闲态。如果按键电平为高，说明按钮释放了没人取消该事件，说明逻辑有问题，报错，并返回空闲态。
 
 上述逻辑报错的print需要打印哪个函数报错。按钮事件输出时采用该机制
 tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK
 tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK)
 tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS
 
 
 
 
 
 */
 
 
 #endif /* INCLUDE_KEY_H_ */
 