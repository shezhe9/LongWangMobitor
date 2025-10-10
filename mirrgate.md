# 工程架构对比与迁移指南

## 📊 两个工程的对比分析

### 1. **串口打印逻辑对比**

#### **LongWangMonitor** (BLE Central 工程)
- **打印方式**: 直接使用 `PRINT()` 宏
  - `PRINT()` 直接映射到 `printf()`，通过 UART1 实时输出
  - 所有日志**即时发送**到串口，没有缓冲
  - 简单直接，适合调试
  - **缺点**: 在中断或关键路径中会阻塞，影响实时性

```c
// LongWangMonitor 的打印示例
PRINT("%s\n", VER_LIB);
PRINT("Key_Init()\n");
PRINT("按键单击事件\n");
```

#### **BLE-HID-OTA** (BLE Peripheral 工程)
- **打印方式**: 混合使用 `PRINT()` + `ulog` 日志系统
  - `PRINT()` 用于**关键启动信息**（即时输出）
  - `ulog` 系统用于**运行时日志**（带缓冲、分级）
  
- **ulog 日志框架特点**:
  ```c
  // 分级日志宏
  utrace()  // 追踪级别
  udebug()  // 调试级别
  uinfo()   // 信息级别（最常用）
  uwarn()   // 警告级别
  uerror()  // 错误级别
  ufatal()  // 致命错误级别
  ```

- **缓冲机制**:
  - 4KB 循环缓冲区 (`ULOG_BUFFER_SIZE 4096`)
  - 每条日志最大 128 字节
  - **异步发送**：通过 TMOS 定时任务每秒处理一次日志队列
  - 避免在中断或关键路径中阻塞

```c
// BLE-HID-OTA 的打印示例
PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);  // 启动信息
uinfo("USB_HID_ENABLE = %d, BLE_HID_ENABLE = %d\n", ...);  // 运行时日志
uinfo("HID 模式:已初始化 HidDev 服务\n");
```

---

### 2. **主程序架构对比**

| 对比维度 | LongWangMonitor (Central) | BLE-HID-OTA (Peripheral) |
|---------|---------------------------|--------------------------|
| **BLE 角色** | Central (主机) | Peripheral (从机) |
| **主要功能** | 扫描连接特定外设 | HID 设备 + OTA 升级 |
| **初始化顺序** | 简洁清晰 | 复杂多模块 |
| **任务数量** | ~3-4 个任务 | ~10+ 个任务 |
| **OTA 支持** | ❌ 无 | ✅ 有完整 OTA 框架 |
| **存储管理** | ❌ 无 | ✅ EEPROM + FlashDB |
| **显示输出** | ❌ 无 | ✅ OLED 显示 |
| **传感器** | ❌ 无 | ✅ SHT30(温湿度) + ADC |
| **日志系统** | 简单 PRINT | 完整 ulog 框架 |

---

### 3. **初始化流程对比**

#### **LongWangMonitor** - 简洁型
```c
int main(void) {
    // 1. 硬件基础
    PWR_DCDCCfg(ENABLE);
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    
    // 2. UART 调试
    UART1_DefInit();
    
    // 3. BLE + HAL
    app_uart_init();
    CH58X_BLEInit();
    HAL_Init();
    
    // 4. BLE Central 角色
    GAPRole_CentralInit();
    Central_Init();
    
    // 5. 按键驱动
    Key_Init();
    
    // 6. 进入主循环
    Main_Circulation();  // TMOS_SystemProcess()
}
```

#### **BLE-HID-OTA** - 完整型
```c
int main(void) {
    // 1. 硬件基础
    PWR_DCDCCfg(ENABLE);
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    
    // 2. UART 调试
    UART1_DefInit();
    uart_hard_init();
    
    // 3. 外设初始化
    PWM_INI();
    ws2812_hard_ini();       // RGB LED
    i2c_app_init(SHT30_ADDR); // 温湿度传感器
    OLED_Display_Init();      // OLED 显示
    
    // 4. BLE + HAL
    ReadImageFlag();  // OTA 镜像检查
    CH58X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    
    // 5. 日志系统
    ulog_buffer_init();
    
    // 6. EEPROM 读取配置
    vm_read(VM_USB_HID_ENABLE, ...);
    vm_read(VM_BLE_HID_ENABLE, ...);
    
    // 7. 条件初始化
    if(BLE_HID_ENABLE) HidDev_Init();
    if(USB_HID_ENABLE) USB_HID_Init();
    
    // 8. 多任务初始化
    HidEmu_Init();
    creat_ad_task();
    ws2812_task_ini();
    Key_Init();
    cold_ctrl_task_ini();
    fan_ctrl_task_ini();
    bump_ctrl_task_ini();
    main_ulog_init();
    uart_task_init();
    cmd_parsing_task_init();
    cmd_send_task_init();
    
    // 9. 配置加载 + 任务启动
    ini_all_cfg();
    start_task();
    
    // 10. 进入主循环
    Main_Circulation();  // TMOS_SystemProcess()
}
```

---

### 4. **底层框架共同点**

两个工程都基于 **WCH CH58x 芯片** + **TMOS 实时操作系统**：

- **TMOS (Task Management OS)**:
  - 轻量级协作式调度器
  - 事件驱动架构
  - 核心循环: `TMOS_SystemProcess()`
  
- **任务注册机制**:
  ```c
  // 注册任务获得 TaskID
  taskID = TMOS_ProcessEventRegister(Task_ProcessEvent);
  
  // 启动定时事件
  tmos_start_task(taskID, EVENT_FLAG, delay_ms);
  
  // 设置即时事件
  tmos_set_event(taskID, EVENT_FLAG);
  
  // 任务处理函数
  uint16_t Task_ProcessEvent(uint8_t task_id, uint16_t events) {
      if(events & EVENT_FLAG) {
          // 处理事件
          return (events ^ EVENT_FLAG);
      }
      return 0;
  }
  ```

---

### 5. **关键差异总结**

| 特性 | LongWangMonitor | BLE-HID-OTA |
|------|----------------|-------------|
| **复杂度** | ⭐⭐ 原型/测试级 | ⭐⭐⭐⭐⭐ 产品级 |
| **日志系统** | 实时 PRINT | 缓冲 ulog |
| **存储管理** | 无 | EEPROM + FlashDB |
| **OTA 升级** | 不支持 | 完整支持 |
| **多模块控制** | 无 | PWM/风扇/水泵/温控 |
| **配置管理** | 硬编码 | EEPROM 持久化 |
| **错误处理** | 简单打印 | 分级日志 + 状态机 |

---

## 🚀 ulog 日志系统迁移指南

### 迁移目标
将 BLE-HID-OTA 的 ulog 缓冲日志系统移植到 LongWangMonitor，实现：
- ✅ 非阻塞日志输出
- ✅ 分级日志管理
- ✅ 循环缓冲区防止内存溢出
- ✅ 在中断和关键路径中安全使用

### 需要复制的文件
1. `Driver/ulog/ulog.c` - ulog 核心实现
2. `Driver/ulog/ulog_buffer.c` - 缓冲管理实现
3. `Driver/ulog/inc/ulog.h` - ulog 接口定义
4. `Driver/ulog/inc/ulog_buffer.h` - 缓冲接口定义
5. `Driver/ulog/inc/ulog_config.h` - 配置文件（需创建）

### 修改步骤

#### 1. 创建 ulog_config.h 配置文件
```c
#ifndef ULOG_CONFIG_H_
#define ULOG_CONFIG_H_

// 定义默认日志级别
#define ULOG_DEFAULT_LEVEL ULOG_LEVEL_TRACE

// 使能 ulog 功能
#define ULOG_ENABLED

#endif
```

#### 2. 在 main 函数中初始化 ulog
```c
#include "ulog_buffer.h"

int main(void) {
    // ... 硬件初始化 ...
    
    UART1_DefInit();
    PRINT("%s\n", VER_LIB);  // 启动信息仍使用 PRINT
    
    // 初始化 ulog 缓冲系统
    ulog_buffer_init();
    uinfo("ulog 系统初始化完成\n");
    
    // ... 其他初始化 ...
}
```

#### 3. 替换日志调用
```c
// 旧代码：
PRINT("按键单击事件\n");
PRINT("连接成功, handle=%d\n", handle);

// 新代码：
uinfo("按键单击事件\n");
uinfo("连接成功, handle=%d\n", handle);

// 关键启动信息仍使用 PRINT：
PRINT("System Start\n");  // 立即输出，确保能看到

// 运行时信息使用 ulog：
uinfo("扫描开始\n");
udebug("调试信息: state=%d\n", state);
uerror("错误: 连接失败\n");
```

#### 4. 使用建议
- **启动阶段**: 使用 `PRINT()` 确保能看到关键信息
- **运行阶段**: 使用 `uinfo()` / `udebug()` / `uerror()` 等
- **中断中**: 只能使用 `ulog`（非阻塞），不能用 `PRINT`
- **调试时**: 可临时提升日志级别到 `ULOG_LEVEL_TRACE`

---

## 📝 迁移日志

### 当前状态
- [x] 完成工程架构分析
- [ ] 复制 ulog 文件到 LongWangMonitor
- [ ] 修改编译配置
- [ ] 初始化 ulog 系统
- [ ] 替换关键路径的 PRINT 为 ulog
- [ ] 测试验证

### 预期收益
1. **性能提升**: 消除串口阻塞，提高响应速度
2. **稳定性**: 在中断中安全记录日志
3. **可维护性**: 分级日志便于问题定位
4. **生产就绪**: 可通过配置关闭详细日志

---

*文档生成时间: 2025-10-10*
*作者: AI Assistant*

