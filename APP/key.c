/*
 * key.c - 多按键管理系统实现
 *
 * 架构说明：
 * - 中断 + 硬件RTC计时 + TMOS定时器
 * - 支持10个按键独立管理
 * - 每个按键独立状态机（三态）
 * - 双边沿触发（按下/释放）
 * - 支持单击、双击、长按检测
 */

#include "CONFIG.h"
#include "key.h"
#include "central.h"
#include "ulog_buffer.h"

/******************** 全局变量 ********************/

// TMOS任务ID
uint8_t keyTaskId = 0xFF;
uint8_t mode_type_golbal= 0x81;
// 按键对象数组（管理10个按键）
KeyObject_t keys[MAX_KEYS];

// 当前活动的按键ID（用于事件分发）
uint8_t activeKeyId = 0xFF;

// 按键配置表（引脚映射）
static const uint16_t keyPinMap[MAX_KEYS] = {
    KEY_PIN_0,   // PA4
    KEY_PIN_1,   // PA5
    KEY_PIN_2,   // PA6
    KEY_PIN_3,   // PA0
    KEY_PIN_4,   // PA1
    KEY_PIN_5,   // PA2
    KEY_PIN_6,   // PA3
    KEY_PIN_7,   // PA15
    KEY_PIN_8,   // PA14
    KEY_PIN_9,   // PA7
    KEY_PIN_10   // PB1
};

// GPIO端口配置表（0=GPIOA, 1=GPIOB）
static const uint8_t keyGpioMap[MAX_KEYS] = {
    KEY_GPIO_A,  // KEY_0: PA4
    KEY_GPIO_A,  // KEY_1: PA5
    KEY_GPIO_A,  // KEY_2: PA6
    KEY_GPIO_A,  // KEY_3: PA0
    KEY_GPIO_A,  // KEY_4: PA1
    KEY_GPIO_A,  // KEY_5: PA2
    KEY_GPIO_A,  // KEY_6: PA3
    KEY_GPIO_A,  // KEY_7: PA15
    KEY_GPIO_A,  // KEY_8: PA14
    KEY_GPIO_A,  // KEY_9: PA7
    KEY_GPIO_B   // KEY_10: PB1
};

// 按键使能配置表
static const uint8_t keyEnableMap[MAX_KEYS] = {
    KEY_ENABLE_0,
    KEY_ENABLE_1,
    KEY_ENABLE_2,
    KEY_ENABLE_3,
    KEY_ENABLE_4,
    KEY_ENABLE_5,
    KEY_ENABLE_6,
    KEY_ENABLE_7,
    KEY_ENABLE_8,
    KEY_ENABLE_9,
    KEY_ENABLE_10
};

/******************** RTC时间戳函数 ********************/

/**
 * @brief 获取当前RTC时间戳
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
 * @param new 新时间戳
 * @param old 旧时间戳
 * @return int32_t 时间差（毫秒）
 */
int32_t TimeDiff(volatile TimeStamp32 *new, volatile TimeStamp32 *old)
{
    int32_t diff = 0;
    diff += (new->day - old->day) * 86400000L;        // 天转毫秒
    diff += (new->sec2 - old->sec2) * 2000;           // 2秒转毫秒
    diff += (new->t32k - old->t32k) * 1000 / 32768;   // 32K计数转毫秒
    return diff;
}

/**
 * @brief 打印时间戳（调试用）
 * @param time 时间戳指针
 */
void print_timestamp(TimeStamp32 *time)
{
    uinfo("R16_RTC_CNT_2S=%d  32K=%d\n", time->sec2, time->t32k);
}

/******************** 按键处理函数 ********************/

/**
 * @brief 按键中断处理函数（核心状态机）
 * @param keyId 按键ID（0-9）
 * 
 * @note 此函数在GPIO中断中被调用，实现三态状态机
 */
void ButtonHandler(uint8_t keyId)
{
    if(keyId >= MAX_KEYS || !keys[keyId].enabled) {
        return;  // 非法ID或未启用
    }
    
    KeyObject_t *key = &keys[keyId];
    TimeStamp32 currentTime;
    
    // 读取当前引脚电平（0=高电平未按下，1=低电平按下）
    uint32_t pinLevel;
    if(key->gpio == KEY_GPIO_A) {
        pinLevel = R32_PA_PIN & key->pin;  // GPIOA
    } else {
        pinLevel = R32_PB_PIN & key->pin;  // GPIOB
    }
    uint32_t buttonPressed = pinLevel ? 0 : 1;
    
    // 三态状态机处理
    if((key->state == BUTTON_IDLE) && (buttonPressed == 1))  // 空闲态下按下
    {
        GetTimeStamp(&key->pressTime);
        key->state = BUTTON_PRESSED;
        
        // 启动长按检测定时器（1.5s）
        activeKeyId = keyId;
        tmos_start_task(keyTaskId, KEY_LONG_PRESSED_CHECK, KEY_LONG_PRESS_TICKS);
    }
    else if((key->state == BUTTON_PRESSED) && (buttonPressed == 0))  // 按下态等释放
    {
        // 取消长按检测事件
        tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);
        
        GetTimeStamp(&key->releaseTime);
        key->pressDuration = TimeDiff(&key->releaseTime, &key->pressTime);
        
        activeKeyId = keyId;
        
        if(key->pressDuration < KEY_DEBOUNCE_TIME)  // 按键抖动，忽略
        {
            tmos_set_event(keyTaskId, KEY_NOISE_PRESSED);
            key->state = BUTTON_IDLE;
        }
        else if(key->pressDuration < SINGLE_CLICK_TIME)  // 可能是单击或双击
        {
            // 进入等待双击态
            key->state = BUTTON_DOUBLE_CLICK_WAIT;
            // 启动双击检测事件（600ms超时）
            tmos_start_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK, KEY_DOUBLE_CLICK_TICKS);
        }
        else if(key->pressDuration < LONG_PRESS_TIME)  // 单击
        {
            tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
            key->state = BUTTON_IDLE;
        }
        else  // 长按已由定时器处理
        {
            tmos_stop_task(keyTaskId, KEY_LONG_PRESSED_CHECK);
            tmos_set_event(keyTaskId, BUTTON_PRESSED_OVERTIME_ERR);
            tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
            key->state = BUTTON_IDLE;
        }
    }
    else if((key->state == BUTTON_DOUBLE_CLICK_WAIT) && (buttonPressed == 1))  // 等待双击态下又按下
    {
        // 取消双击检测事件
        tmos_stop_task(keyTaskId, KEY_DOUBLE_CLICK_CHECK);
        
        GetTimeStamp(&currentTime);
        int32_t timeSinceRelease = TimeDiff(&currentTime, &key->releaseTime);
        
        activeKeyId = keyId;
        
        if(timeSinceRelease < DOUBLE_CLICK_TIME)  // 双击成立
        {
            // 触发双击事件
            tmos_set_event(keyTaskId, KEY_EVENT_DOUBLE_CLICK);
            // 回到空闲态
            key->state = BUTTON_IDLE;
        }
        else  // 超时，报错
        {
            tmos_set_event(keyTaskId, DOUBULE_PRESSED_OVERTIME_ERR);
            key->state = BUTTON_IDLE;
        }
    }
    else  // 其他异常状态
    {
        uint16_t unknownState = KEY_STATE_UNKOWN;
        
        if((key->state == BUTTON_IDLE) && (buttonPressed == 0)) {
            unknownState = SINGLE_PRESSED_RELASE;  // 空闲态按钮释放
        }
        if((key->state == BUTTON_DOUBLE_CLICK_WAIT) && (buttonPressed == 0)) {
            unknownState = DOUBULE_PRESSED_RELASE;  // 双击后释放按键
        }
        
        activeKeyId = keyId;
        tmos_set_event(keyTaskId, unknownState);
        key->state = BUTTON_IDLE;
    }
}

/******************** GPIO中断处理 ********************/

/**
 * @brief GPIOA中断服务函数（处理GPIOA上的按键）
 * 
 * @note 此函数会遍历所有在GPIOA上的启用按键，检查哪个触发了中断
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOA_IRQHandler(void)
{
    // 遍历所有按键，检查中断标志
    for(uint8_t i = 0; i < MAX_KEYS; i++)
    {
        if(!keys[i].enabled || keys[i].gpio != KEY_GPIO_A) continue;  // 跳过未启用或非GPIOA的按键
        
        if(R16_PA_INT_IF & keys[i].pin)  // 检查中断标志
        {
            // 清除中断标志
            R16_PA_INT_IF |= keys[i].pin;
            
            // 切换触发边沿（关键：实现双边沿检测）
            if(keys[i].triggerMode == GPIO_ITMode_FallEdge)
            {
                GPIOA_ITModeCfg(keys[i].pin, GPIO_ITMode_RiseEdge);
                keys[i].triggerMode = GPIO_ITMode_RiseEdge;
            }
            else
            {
                GPIOA_ITModeCfg(keys[i].pin, GPIO_ITMode_FallEdge);
                keys[i].triggerMode = GPIO_ITMode_FallEdge;
            }
            
            // 处理按键事件
            ButtonHandler(i);
        }
    }
}

/**
 * @brief GPIOB中断服务函数（处理GPIOB上的按键）
 * 
 * @note 此函数会遍历所有在GPIOB上的启用按键，检查哪个触发了中断
 */
__attribute__((interrupt("WCH-Interrupt-fast")))
void GPIOB_IRQHandler(void)
{
    // 遍历所有按键，检查中断标志
    for(uint8_t i = 0; i < MAX_KEYS; i++)
    {
        if(!keys[i].enabled || keys[i].gpio != KEY_GPIO_B) continue;  // 跳过未启用或非GPIOB的按键
        
        if(R16_PB_INT_IF & keys[i].pin)  // 检查中断标志
        {
            // 清除中断标志
            R16_PB_INT_IF |= keys[i].pin;
            
            // 切换触发边沿（关键：实现双边沿检测）
            if(keys[i].triggerMode == GPIO_ITMode_FallEdge)
            {
                GPIOB_ITModeCfg(keys[i].pin, GPIO_ITMode_RiseEdge);
                keys[i].triggerMode = GPIO_ITMode_RiseEdge;
            }
            else
            {
                GPIOB_ITModeCfg(keys[i].pin, GPIO_ITMode_FallEdge);
                keys[i].triggerMode = GPIO_ITMode_FallEdge;
            }
            
            // 处理按键事件
            ButtonHandler(i);
        }
    }
}

/******************** 初始化函数 ********************/

/**
 * @brief 初始化按键系统（支持多按键）
 */
void Key_Init(void)
{
    // 注册TMOS任务
    keyTaskId = TMOS_ProcessEventRegister(Key_ProcessEvent);
    uinfo("\260\264\274\374\317\265\315\263\263\365\312\274\273\257: keyTaskId=%d\n", keyTaskId);  // 按键系统初始化
    
    // 初始化所有按键对象
    uint16_t enabledPinsA = 0;  // GPIOA启用的引脚
    uint16_t enabledPinsB = 0;  // GPIOB启用的引脚
    uint8_t enabledCount = 0;
    
    for(uint8_t i = 0; i < MAX_KEYS; i++)
    {
        keys[i].pin = keyPinMap[i];
        keys[i].gpio = keyGpioMap[i];
        keys[i].enabled = keyEnableMap[i];
        keys[i].state = BUTTON_IDLE;
        keys[i].triggerMode = GPIO_ITMode_FallEdge;
        keys[i].pressTime = (TimeStamp32){0, 0, 0};
        keys[i].releaseTime = (TimeStamp32){0, 0, 0};
        keys[i].pressDuration = 0;
        
        if(keys[i].enabled)
        {
            if(keys[i].gpio == KEY_GPIO_A) {
                enabledPinsA |= keys[i].pin;
                uinfo("\260\264\274\374[%d] \306\364\323\303 PA (0x%04x)\n", i, keys[i].pin);  // 按键[X] 启用 PA
            } else {
                enabledPinsB |= keys[i].pin;
                uinfo("\260\264\274\374[%d] \306\364\323\303 PB (0x%04x)\n", i, keys[i].pin);  // 按键[X] 启用 PB
            }
            enabledCount++;
        }
    }
    
    // 配置GPIOA（如果有启用的引脚）
    if(enabledPinsA > 0)
    {
        uinfo("\311\350\326\303 GPIOA \316\252\312\344\310\353\304\243\312\275(GPIO_ModeIN_PU): 0x%04x\n", enabledPinsA);
        GPIOA_ModeCfg(enabledPinsA, GPIO_ModeIN_PU);
        
        // 配置中断（下降沿触发）
        uinfo("\311\350\326\303 GPIOA \316\252\317\302\275\265\321\330\264\245\267\242(GPIO_ITMode_FallEdge): 0x%04x\n", enabledPinsA);
        GPIOA_ITModeCfg(enabledPinsA, GPIO_ITMode_FallEdge);
        
        // 启用GPIOA中断
        PFIC_EnableIRQ(GPIO_A_IRQn);
        uinfo("\306\364\323\303 GPIOA \326\320\266\317(GPIO_A_IRQn)\n");  // 启用GPIOA中断
    }
    
    // 配置GPIOB（如果有启用的引脚）
    if(enabledPinsB > 0)
    {
        uinfo("\311\350\326\303 GPIOB \316\252\312\344\310\353\304\243\312\275(GPIO_ModeIN_PU): 0x%04x\n", enabledPinsB);
        GPIOB_ModeCfg(enabledPinsB, GPIO_ModeIN_PU);
        
        // 配置中断（下降沿触发）
        uinfo("\311\350\326\303 GPIOB \316\252\317\302\275\265\321\330\264\245\267\242(GPIO_ITMode_FallEdge): 0x%04x\n", enabledPinsB);
        GPIOB_ITModeCfg(enabledPinsB, GPIO_ITMode_FallEdge);
        
        // 启用GPIOB中断
        PFIC_EnableIRQ(GPIO_B_IRQn);
        uinfo("\306\364\323\303 GPIOB \326\320\266\317(GPIO_B_IRQn)\n");  // 启用GPIOB中断
    }
    
    // 初始化活动按键ID
    activeKeyId = 0xFF;
    
    uinfo("\260\264\274\374\317\265\315\263\263\365\312\274\273\257\315\352\263\311! \262\341\327\367 %d \270\366 %d \270\366\260\264\274\374\n", 
          MAX_KEYS, 
          enabledCount);  // 按键系统初始化完成！
}

/******************** TMOS事件处理函数 ********************/

/**
 * @brief TMOS任务处理按键事件
 * @param taskId 任务ID
 * @param events 事件标志
 * @return uint16_t 未处理的事件
 */
uint16_t Key_ProcessEvent(uint8_t taskId, uint16_t events)
{
    // 处理单击事件
    if(events & KEY_EVENT_SINGLE_CLICK)
    {
        if(activeKeyId < MAX_KEYS) {
            uinfo("\260\264\274\374[%d] \265\245\273\367\312\302\274\376\n", activeKeyId);  // 按键[X]单击事件
            
            // 按键功能实现
            switch(activeKeyId) {
                case 2: {  // 按键[2]单击事件：触发MSG_DISCONNECT事件
                    uinfo("\260\264\274\374[2]: \264\245\267\242\266\317\277\252\301\254\275\323\n");  // 按键触发断开连接
                    // 调用断开连接函数
                    Central_DisconnectAndStopAutoReconnect();
                    break;
                }
                
                case 3: {  // 按键[3]单击事件：触发MSG_RECONNECT事件
                    uinfo("\260\264\274\374[3]: \264\245\267\242\326\330\320\302\301\254\275\323\n");  // 按键触发重新连接
                    // 调用重新连接函数
                    Central_StartAutoReconnect();
                    break;
                }
                
                case 10: {  // 按键[10]单击事件：轮换发送三个命令
                    if(centralState == BLE_STATE_CONNECTED && centralConnHandle != GAP_CONNHANDLE_INIT && centralWriteCharHdl != 0) {
                        // 检查是否有其他GATT操作正在进行
                        if(centralProcedureInProgress == TRUE) {
                            uinfo("GATT \262\331\327\367\325\375\324\332\275\370\320\320\326\320,\316\336\267\250\267\242\313\315\303\374\301\356\n");  // 操作正在进行中无法发送命令
                            return (events ^ KEY_EVENT_SINGLE_CLICK);
                        }
                        
                        //static uint8_t cmd_index = 0;
                        attWriteReq_t req;
                        req.cmd = TRUE;
                        req.sig = FALSE;
                        req.handle = centralWriteCharHdl;
                        req.len = 3;  // 3字节命令
                        req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
                        
                        if(req.pValue != NULL) {
                            // 固定的命令格式部分
                            req.pValue[1] = 0x00;
                            req.pValue[2] = 0x01;
                            
                            // 更新命令索引，实现轮换
                            //mode_type_golbal = (mode_type_golbal + 1);
                            //if(mode_type_golbal > 0x83)
                            //{
                            //    mode_type_golbal = 0x81;
                            //}
                            // 根据当前索引选择不同的命令
                            switch(mode_type_golbal) {
                                case 0x83:
                                    req.pValue[0] = 0x81;
                                    uinfo("\260\264\274\374[10]: \267\242\313\315\303\374\301\356 81 00 01 (\303\374\301\3561)\n");  // 按键发送命令命令
                                    break;
                                case 0x81:
                                    req.pValue[0] = 0x82;
                                    uinfo("\260\264\274\374[10]: \267\242\313\315\303\374\301\356 82 00 01 (\303\374\301\3562)\n");  // 按键发送命令命令
                                    break;
                                case 0x82:
                                    req.pValue[0] = 0x83;
                                    uinfo("\260\264\274\374[10]: \267\242\313\315\303\374\301\356 83 00 01 (\303\374\301\3563)\n");  // 按键发送命令命令
                                    break;
                                default:
                                   
                                    uinfo( "mode_type_golbal=%d\n", mode_type_golbal);  // 按键发送命令命令
                                    break;
                            }
                            
                            bStatus_t status = GATT_WriteNoRsp(centralConnHandle, &req);
                            if(status != SUCCESS) {
                                GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
                                uinfo("\267\242\313\315\312\247\260\334,\327\264\314\254: 0x%02X\n", status);  // 发送失败状态
                            }
                            
                            
                        }
                    } else {
                        uinfo("BLE \316\264\301\254\275\323,\316\336\267\250\267\242\313\315\303\374\301\356\n");  // 未连接无法发送命令
                    }
                    break;
                }
            }
        }
        return (events ^ KEY_EVENT_SINGLE_CLICK);
    }
    
    // 处理双击事件
    if(events & KEY_EVENT_DOUBLE_CLICK)
    {
        if(activeKeyId < MAX_KEYS) {
            uinfo("\260\264\274\374[%d] \313\253\273\367\312\302\274\376\n", activeKeyId);  // 按键[X]双击事件
            
            // 按键0的双击功能：发送测试数据
            if(activeKeyId == 0) {
                tmos_start_task(centralTaskId, START_SEND_TEST_DATA_EVT, 10);
            }
        }
        return (events ^ KEY_EVENT_DOUBLE_CLICK);
    }
    
    // 处理长按事件
    if(events & KEY_EVENT_LONG_PRESS)
    {
        if(activeKeyId < MAX_KEYS) {
            uinfo("\260\264\274\374[%d] \263\244\260\264\312\302\274\376\n", activeKeyId);  // 按键[X]长按事件
            
            // 按键6的长按功能：发送重置从机的命令 a2 00 01 00
            if(activeKeyId == 6) {

                attWriteReq_t req;
                req.cmd = TRUE;
                req.sig = FALSE;
                req.handle = centralWriteCharHdl;
                req.len = 4;  // 4字节命令
                req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_CMD, req.len, NULL, 0);
                
                if(req.pValue != NULL) {
                    req.pValue[0] = 0xa2;
                    req.pValue[1] = 0x00;
                    req.pValue[2] = 0x01;
                    req.pValue[3] = 0x00;
                    
                    bStatus_t status = GATT_WriteNoRsp(centralConnHandle, &req);
                    if(status != SUCCESS) {
                        GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_CMD);
                        uinfo("\267\242\313\315\312\247\260\334,\327\264\314\254: 0x%02X\n", status);  // 发送失败状态
                    }
                }
            }
        }   
        return (events ^ KEY_EVENT_LONG_PRESS);
    }
    
    // 双击检测事件（超时检测）
    if(events & KEY_DOUBLE_CLICK_CHECK)
    {
        if(activeKeyId < MAX_KEYS && keys[activeKeyId].enabled)
        {
            KeyObject_t *key = &keys[activeKeyId];
            uint32_t pinLevel;
            if(key->gpio == KEY_GPIO_A) {
                pinLevel = R32_PA_PIN & key->pin;
            } else {
                pinLevel = R32_PB_PIN & key->pin;
            }
            uint8_t buttonLevel = pinLevel ? 0 : 1;
            
            if(key->state == BUTTON_DOUBLE_CLICK_WAIT)
            {
                if(buttonLevel == 0)  // 按键未按下，确认为单击
                {
                    tmos_set_event(keyTaskId, KEY_EVENT_SINGLE_CLICK);
                    key->state = BUTTON_IDLE;
                }
                else  // 按键已按下，报错
                {
                    uwarn("\260\264\274\374[%d] \313\253\273\367\270\264\262\342\320\363\263\243: \260\264\274\374\322\321\260\264\317\302\265\253\326\320\266\317\316\264\264\245\267\242\n", 
                          activeKeyId);  // 双击复测异常
                    key->state = BUTTON_IDLE;
                }
            }
            else
            {
                uwarn("\260\264\274\374[%d] \313\253\273\367\270\264\262\342\320\363\263\243: \267\307\265\310\264\375\313\253\273\367\327\264\314\254 %d\n", 
                      activeKeyId, key->state);  // 非等待双击状态
            }
        }
        
        return (events ^ KEY_DOUBLE_CLICK_CHECK);
    }
    
    // 长按检测事件（超时检测）
    if(events & KEY_LONG_PRESSED_CHECK)
    {
        if(activeKeyId < MAX_KEYS && keys[activeKeyId].enabled)
        {
            KeyObject_t *key = &keys[activeKeyId];
            uint32_t pinLevel;
            if(key->gpio == KEY_GPIO_A) {
                pinLevel = R32_PA_PIN & key->pin;
            } else {
                pinLevel = R32_PB_PIN & key->pin;
            }
            uint8_t buttonLevel = pinLevel ? 0 : 1;
            
            if(key->state == BUTTON_PRESSED)
            {
                if(buttonLevel == 1)  // 按键仍处于按下状态
                {
                    // 触发长按事件
                    tmos_set_event(keyTaskId, KEY_EVENT_LONG_PRESS);
                    
                    // 设置为下降沿触发，准备下一次按下
                    if(key->gpio == KEY_GPIO_A) {
                        GPIOA_ITModeCfg(key->pin, GPIO_ITMode_FallEdge);
                    } else {
                        GPIOB_ITModeCfg(key->pin, GPIO_ITMode_FallEdge);
                    }
                    key->triggerMode = GPIO_ITMode_FallEdge;
                    
                    // 恢复空闲状态
                    key->state = BUTTON_IDLE;
                }
                else  // 按键已释放但没有触发中断
                {
                    uerror("\260\264\274\374[%d] \263\244\260\264\270\264\262\342\320\363\263\243: \260\264\274\374\322\321\312\315\267\305\265\253\326\320\266\317\316\264\264\245\267\242\n", 
                           activeKeyId);  // 长按复测异常
                    key->state = BUTTON_IDLE;
                }
            }
            else
            {
                uerror("\260\264\274\374[%d] \263\244\260\264\270\264\262\342\320\363\263\243: \267\307\260\264\317\302\327\264\314\254 %d\n", 
                       activeKeyId, key->state);  // 非按下状态
            }
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
        uinfo("\312\261\274\344: %d \314\354 %d \303\353", day, sec2*2+i);  // 时间
        
        // 打印所有按键状态
        for(uint8_t j = 0; j < MAX_KEYS; j++)
        {
            if(keys[j].enabled) {
                uinfo(" Key%d:%d", j, keys[j].state);
            }
        }
        uinfo("\n");
        
        tmos_start_task(keyTaskId, KEY_TEST_SECOND, 1600);  // 1s定时
        return (events ^ KEY_TEST_SECOND);
    }
    
    // 按键抖动事件
    if(events & KEY_NOISE_PRESSED)
    {
        if(activeKeyId < MAX_KEYS) {
            udebug("\260\264\274\374[%d] \266\266\266\257, \272\366\302\324\264\313\264\316\312\302\274\376\n", activeKeyId);  // 按键抖动
        }
        return (events ^ KEY_NOISE_PRESSED);
    }
    
    // 按键状态未知错误
    if(events & KEY_STATE_UNKOWN)
    {
        if(activeKeyId < MAX_KEYS) {
            uwarn("\260\264\274\374[%d] \327\264\314\254\322\354\263\243, \273\330\265\275\277\325\317\320\314\254\n", activeKeyId);  // 按键状态异常
        }
        return (events ^ KEY_STATE_UNKOWN);
    }
    
    // 按下超时错误
    if(events & BUTTON_PRESSED_OVERTIME_ERR)
    {
        if(activeKeyId < MAX_KEYS) {
            uerror("\260\264\274\374[%d] \260\264\317\302\263\254\312\261: pressDuration=%d ms\n", 
                   activeKeyId, keys[activeKeyId].pressDuration);  // 按键按下超时
            
            print_timestamp(&keys[activeKeyId].pressTime);
            print_timestamp(&keys[activeKeyId].releaseTime);
        }
        return (events ^ BUTTON_PRESSED_OVERTIME_ERR);
    }
    
    // 双击超时错误
    if(events & DOUBULE_PRESSED_OVERTIME_ERR)
    {
        if(activeKeyId < MAX_KEYS) {
            uwarn("\260\264\274\374[%d] \313\253\273\367\263\254\312\261, \323\246\316\252\301\355\315\342\322\273\264\316\265\245\273\367\n", activeKeyId);  // 双击超时
        }
        return (events ^ DOUBULE_PRESSED_OVERTIME_ERR);
    }
    
    // 单击释放事件
    if(events & SINGLE_PRESSED_RELASE)
    {
        if(activeKeyId < MAX_KEYS) {
            udebug("\260\264\274\374[%d] \265\245\273\367\312\315\267\305\n", activeKeyId);  // 单击释放
        }
        return (events ^ SINGLE_PRESSED_RELASE);
    }
    
    // 双击释放事件
    if(events & DOUBULE_PRESSED_RELASE)
    {
        if(activeKeyId < MAX_KEYS) {
            udebug("\260\264\274\374[%d] \313\253\273\367\312\315\267\305\n", activeKeyId);  // 双击释放
        }
        return (events ^ DOUBULE_PRESSED_RELASE);
    }
    
    // 返回未处理的事件
    return 0;
}

/**
 * @brief 获取按键事件（兼容旧版接口）
 * @return 按键事件
 */
KeyEvent Key_GetEvent(void)
{
    // 此函数为兼容接口，实际使用TMOS事件机制
    return KEY_EVENT_NONE;
}
