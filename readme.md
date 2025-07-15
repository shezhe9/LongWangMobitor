# CH582M HID控制项目

该项目是基于沁恒的CH582M的HID操控项目。项目把CH582M作为主设备（BLE Central），可以发现并连接指定的HID外设设备。

## 项目结构

```
ch582test/
├── APP/                          # 应用层代码
│   ├── central_main.c           # 主程序入口，系统初始化
│   ├── central.c/central.h      # BLE Central核心功能实现
│   ├── uart_cmd.c/uart_cmd.h    # UART命令处理模块
│   ├── key.c/key.h              # 按键控制模块(状态机实现)
│   └── ws2812.c/ws2812.h        # WS2812 LED状态指示模块
├── Profile/                     # BLE配置文件
│   └── include/gattprofile.h    # GATT服务和特征定义
├── obj/                         # 编译输出文件
└── 硬件驱动库/                   # CH582芯片底层驱动
```

## 核心功能模块

### 1. BLE Central模块 (central.c)
- **智能设备发现**: 通过设备名"HID-LongWang"自动识别目标HID设备
- **VendorID/ProductID验证**: 连接后验证VID(0x08D8)/PID(0x0001)匹配
- **自动连接管理**: 失败时自动重新扫描连接
- **GATT服务发现**: 自动发现HID服务(0x1812)和相关特征
- **数据传输**: 支持20字节HID数据发送和接收

### 2. UART命令模块 (uart_cmd.c)
支持以下串口命令：
- `0x31`: 切换CH串口开关
- `0x32`: 切换ESP烧录口开关  
- `0x33`: 切换本机ESP烧录串口开关
- `0x34`: 切换ESP串口1日志开关
- `0x35`: 切换温度传感器开关
- `0x38`: 触发按键单击事件
- `0x39`: 系统复位
- **`0x3A`: 发送HID测试数据 (新增)**

### 3. 按键控制模块 (key.c)
- 中断+定时器的按键状态机
- 支持单击、双击、长按检测
- 电源管理控制(3.3V/12V切换)

### 4. LED指示模块 (ws2812.c)
- SPI驱动WS2812灯带
- 不同状态的可视化指示
- 亮度可调节

## HID设备匹配机制

项目支持两种匹配方式：

### 主要匹配方式：设备名匹配
- 扫描过程中解析广播数据
- 匹配设备名：`"HID-LongWang"`
- 优先级最高，发现即连接

### 辅助验证：VendorID/ProductID
- 连接后读取Device Information Service
- 验证VendorID: `0x08D8`
- 验证ProductID: `0x0001`

## HID通信协议

### 数据通道
- **Report ID 1**: (HID外设→本机CH582) (20字节有效载荷)
- **Report ID 2**: (本机CH582→HID外设) (20字节有效载荷)

### 测试数据格式
发送测试数据内容：`01 02 03 04 ... 14` (20个递增字节)

## 使用方法

### 1. 设备连接
1. 上电后系统自动开始扫描HID设备
2. 发现名为"HID-LongWang"的设备后自动连接
3. 连接成功后进行服务发现和特征配置

### 2. 发送测试数据
通过串口发送命令 `0x3A` 触发20字节测试数据发送

### 3. 状态监控
- 通过串口日志查看连接状态和数据传输情况
- WS2812 LED指示不同的工作状态

## 关键特性

- ? **智能设备识别**: 基于设备名自动匹配，无需硬编码MAC地址
- ? **双重验证机制**: 设备名+VID/PID双重确认
- ? **自动重连**: 连接失败或断开后自动重新扫描
- ? **完整的HID支持**: 标准HID服务(0x1812)通信
- ? **串口控制接口**: 便于外部系统控制
- ? **状态可视化**: LED指示各种工作状态

## 技术规格

- **芯片**: 沁恒CH582M
- **协议**: BLE 5.0 Central模式
- **HID服务**: UUID 0x1812
- **数据长度**: 20字节有效载荷
- **连接参数**: 可配置间隔和超时时间

## 项目结构分析

基于我对整个项目的完整阅读，这是一个基于CH582M芯片的BLE Central（中心设备）项目，具有以下结构：

### 核心模块架构

```
<code_block_to_apply_changes_from>
```

### 功能模块详解

1. **BLE Central模块** (`central.c`)
   - 设备扫描与发现
   - 自动连接目标HID设备
   - GATT服务发现
   - 特征值读写操作
   - 连接管理和异常处理

2. **UART命令模块** (`uart_cmd.c`)
   - 接收外部控制命令
   - 支持多种开关控制(0x31-0x39)
   - 看门狗复位控制

3. **按键控制模块** (`key.c`)
   - 中断+定时器的按键状态机
   - 支持单击、双击、长按检测
   - 电源管理控制

4. **LED指示模块** (`ws2812.c`)
   - SPI驱动WS2812灯带
   - 状态可视化指示
   - 亮度可调节

## HID发现和连接功能实现分析

### 1. HID设备发现流程

```c
// 在 central.c 中的设备发现流程：
GAP_DEVICE_INIT_DONE_EVENT -> 
GAPRole_CentralStartDiscovery() -> 
GAP_DEVICE_INFO_EVENT -> centralAddDeviceInfo() ->
GAP_DEVICE_DISCOVERY_EVENT -> 检查目标地址匹配
```

**目标设备配置：**
```c
// 预设的HID设备地址
static uint8_t PeerAddrDef[B_ADDR_LEN] = {0x03, 0x03, 0x03, 0xE4, 0xC2, 0x84};
```

### 2. HID设备连接流程

```c
// 连接建立流程
GAPRole_CentralEstablishLink() -> 
GAP_LINK_ESTABLISHED_EVENT -> 
centralStartDiscovery() -> // 服务发现
GATT_DiscPrimaryServiceByUUID() -> // 发现主服务
GATT_DiscAllChars() -> // 发现所有特征
GATT_ReadUsingCharUUID() // 发现CCCD
```

**使用的服务信息：**
- 服务UUID: `0x1812` (HID Service)
- 特征UUID: `0x2A22` (Boot Keyboard Input Report)
- 支持通知机制(CCCD)

### 3. 数据传输机制

当前项目通过GATT协议与HID设备通信：
- **接收数据**: 通过ATT_HANDLE_VALUE_NOTI事件接收HID设备发送的数据
- **发送数据**: 通过GATT_WriteCharValue()向特征句柄写入数据

## 向HID外设发送20字节测试数据的实现方案

### 方案概述

根据你的需求，需要向HID外设发送内容为`01,02,03,...,20`的20字节测试数据。基于现有代码架构，我建议以下实现方案：

### 1. 添加HID数据发送函数

```c
// 在central.c中添加新的发送函数
bool SendHIDTestData(void)
{
    if(centralState != BLE_STATE_CONNECTED || centralCharHdl == 0) 
    {
        PRINT("HID设备未连接或特征句柄无效\n");
        return false;
    }
    
    if(centralProcedureInProgress == TRUE) 
    {
        PRINT("GATT操作正在进行中，请稍后重试\n");
        return false;
    }

    // 准备20字节测试数据 (01, 02, 03, ..., 20)
    attWriteReq_t req;
    req.cmd = FALSE;
    req.sig = FALSE;
    req.handle = centralCharHdl;  // 使用发现的特征句柄
    req.len = 20;  // 20字节数据长度
    
    req.pValue = GATT_bm_alloc(centralConnHandle, ATT_WRITE_REQ, req.len, NULL, 0);
    if(req.pValue != NULL) 
    {
        // 填充测试数据 01, 02, 03, ..., 20
        for(uint8_t i = 0; i < 20; i++) 
        {
            req.pValue[i] = i + 1;
        }
        
        // 发送数据到HID设备
        if(GATT_WriteCharValue(centralConnHandle, &req, centralTaskId) == SUCCESS) 
        {
            centralProcedureInProgress = TRUE;
            PRINT("20字节测试数据发送成功\n");
            return true;
        }
        else 
        {
            GATT_bm_free((gattMsg_t *)&req, ATT_WRITE_REQ);
            PRINT("数据发送失败\n");
            return false;
        }
    }
    else 
    {
        PRINT("内存分配失败\n");
        return false;
    }
}
```

### 2. 添加触发事件

```c
// 在central.h中添加新事件定义
#define START_HID_TEST_DATA_EVT    0x0400

// 在central.c的事件处理函数中添加
if(events & START_HID_TEST_DATA_EVT) 
{
    SendHIDTestData();
    return (events ^ START_HID_TEST_DATA_EVT);
}
```

### 3. 调用方式

#### 方式1：通过UART命令触发
```c
// 在uart_cmd.c中添加新的命令处理
if (uart_rx_buffer[0] == 0x3A)  // 新增0x3A命令用于发送HID测试数据
{
    PRINT("发送HID测试数据\n");
    tmos_set_event(centralTaskId, START_HID_TEST_DATA_EVT);
}
```

#### 方式2：通过按键触发
```c
// 在key.c的长按事件中添加
if(events & KEY_EVENT_LONG_PRESS)
{
    PRINT("长按触发HID测试数据发送\n");
    tmos_set_event(centralTaskId, START_HID_TEST_DATA_EVT);
    return (events ^ KEY_EVENT_LONG_PRESS);
}
```

#### 方式3：定时自动发送
```c
// 连接成功后启动定时发送
if(pEvent->gap.hdr.status == SUCCESS)
{
    // ... 现有连接处理代码 ...
    
    // 启动5秒后自动发送测试数据
    tmos_start_task(centralTaskId, START_HID_TEST_DATA_EVT, 8000); // 5秒后发送
}
```

### 4. 数据发送确认

发送结果会在`centralProcessGATTMsg()`函数的写入响应处理中确认：

```c
else if((pMsg->method == ATT_WRITE_RSP) || ...)
{
    if(pMsg->method == ATT_ERROR_RSP)
    {
        PRINT("HID数据发送失败: %x\n", status);
    }
    else
    {
        PRINT("HID测试数据发送成功确认\n");
    }
    centralProcedureInProgress = FALSE;
}
```

### 使用说明

1. **连接HID设备**：系统会自动扫描并连接配置的HID设备地址
2. **触发数据发送**：通过串口发送`0x3A`命令或长按按键
3. **确认发送结果**：通过串口输出查看发送状态

这个方案完全基于现有的代码架构，无需大幅修改，只需要添加对应的事件处理和函数即可实现向HID外设发送20字节测试数据的功能。



