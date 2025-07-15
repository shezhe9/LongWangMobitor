
该项目是基于沁恒的CH582M的BLE操控项目。项目把CH582M作为主设备（BLE Central），可以发现并连接指定的BLE外设设备。

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
