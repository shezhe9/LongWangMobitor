#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ESP8266/ESP32 Boot Mode Controller
双击运行即可控制 ESP 芯片进入 Boot 模式

原理说明：
- DTR 信号连接到 GPIO0（低电平=进入下载模式）
- RTS 信号连接到 EN/RESET（低电平=芯片复位）
- 通过特定时序让芯片在 GPIO0 为低时启动，进入 boot 模式
"""

import serial
import serial.tools.list_ports
import time
import sys

def list_serial_ports():
    """列出所有可用的串口"""
    ports = serial.tools.list_ports.comports()
    available_ports = []
    
    print("\n=== 可用串口列表 ===")
    for i, port in enumerate(ports, 1):
        print(f"{i}. {port.device} - {port.description}")
        available_ports.append(port.device)
    
    return available_ports

def enter_boot_mode(port_name):
    """
    控制 ESP8266/ESP32 进入 Boot 模式
    
    时序说明：
    1. DTR=False (GPIO0=HIGH), RTS=True (EN=LOW)  -> 芯片复位
    2. DTR=True (GPIO0=LOW), RTS=False (EN=HIGH)  -> 芯片启动，GPIO0为低，进入boot模式
    3. DTR=False (GPIO0=HIGH)                      -> 恢复GPIO0为高
    
    注意：DTR 和 RTS 是低电平有效信号
    - True = 引脚电压为 0V（低电平）
    - False = 引脚电压为 VCC（高电平）
    """
    try:
        print(f"\n正在打开串口: {port_name}")
        # 打开串口，波特率对控制信号无影响，这里使用常见值
        ser = serial.Serial(port_name, 115200, timeout=1)
        print("串口已打开")
        
        print("\n开始进入 Boot 模式时序...")
        print("(基于 ESP8266 自动下载电路原理)")
        
        # 步骤 1: 芯片复位（EN=LOW）
        print("\n步骤 1: 芯片复位")
        print("  DTR=False(VCC), RTS=True(0V) → EN=0, IO0=1")
        ser.dtr = False  # DTR引脚=VCC
        ser.rts = True   # RTS引脚=0V → EN=0 (芯片复位)
        print("  等待 100ms，让 EN 电容完全放电...")
        time.sleep(0.1)
        
        # 步骤 2: 拉低 GPIO0（IO0=LOW）
        print("\n步骤 2: 拉低 GPIO0")
        print("  DTR=True(0V) → IO0=0")
        ser.dtr = True   # DTR引脚=0V → IO0被拉低
        
        # 步骤 3: 释放复位，EN 开始上升（关键步骤）
        print("\n步骤 3: 释放复位，EN 开始上升（关键步骤）")
        print("  RTS=False(VCC) → EN 从0V开始上升")
        print("  此时 IO0=0，当 EN 上升沿到达阈值时，芯片进入 Boot 模式")
        ser.rts = False  # RTS引脚=VCC → EN开始上升
        print("  等待 100ms，让 EN 完全上升...")
        time.sleep(0.1)
        
        # 步骤 4: 恢复 IO0 为高
        print("\n步骤 4: 恢复 IO0 为高")
        print("  DTR=False(VCC), RTS=False(VCC) → EN=1, IO0=1")
        ser.dtr = False  # DTR引脚=VCC
        # RTS 保持 False(VCC)
        time.sleep(0.05)
        
        print("\n✓ ESP8266/ESP32 已成功进入 Boot 模式！")
        print("现在可以使用烧录工具进行固件下载了。")
        
        ser.close()
        print("\n串口已关闭")
        return True
        
    except serial.SerialException as e:
        print(f"\n✗ 串口错误: {e}")
        print("请确保：")
        print("1. 串口未被其他程序占用")
        print("2. USB 驱动已正确安装")
        print("3. 设备已正确连接")
        return False
    except Exception as e:
        print(f"\n✗ 发生错误: {e}")
        return False

def reset_to_flash_mode(port_name):
    """将 ESP 复位到正常运行模式（Flash 启动模式）"""
    try:
        print(f"\n正在打开串口: {port_name}")
        ser = serial.Serial(port_name, 115200, timeout=1)
        
        print("复位到正常运行模式...")
        ser.dtr = False  # GPIO0=HIGH (正常启动)
        ser.rts = True   # EN=LOW (复位)
        time.sleep(0.1)
        ser.rts = False  # EN=HIGH (启动)
        time.sleep(0.05)
        
        print("✓ ESP8266/ESP32 已复位到正常运行模式")
        
        ser.close()
        return True
        
    except Exception as e:
        print(f"\n✗ 发生错误: {e}")
        return False

def main():
    print("=" * 60)
    print("ESP8266/ESP32 Boot Mode Controller")
    print("ESP 启动模式控制工具")
    print("=" * 60)
    
    # 列出可用串口
    ports = list_serial_ports()
    
    if not ports:
        print("\n✗ 未检测到可用串口！")
        print("请检查：")
        print("1. ESP 设备是否已连接")
        print("2. USB 驱动是否已安装（CH340/CP2102/FTDI等）")
        input("\n按回车键退出...")
        return
    
    # 用户选择串口
    print("\n" + "=" * 60)
    while True:
        try:
            choice = input("请输入串口编号 (直接输入数字，如 1): ").strip()
            port_index = int(choice) - 1
            if 0 <= port_index < len(ports):
                selected_port = ports[port_index]
                break
            else:
                print(f"✗ 无效选择，请输入 1-{len(ports)} 之间的数字")
        except ValueError:
            print("✗ 请输入有效的数字")
    
    # 用户选择操作
    print("\n" + "=" * 60)
    print("请选择操作：")
    print("1. 进入 Boot 模式（用于烧录固件）")
    print("2. 复位到正常运行模式")
    print("=" * 60)
    
    while True:
        mode = input("请输入操作编号 (1 或 2): ").strip()
        if mode in ['1', '2']:
            break
        print("✗ 请输入 1 或 2")
    
    # 执行操作
    print("\n" + "=" * 60)
    if mode == '1':
        success = enter_boot_mode(selected_port)
    else:
        success = reset_to_flash_mode(selected_port)
    
    print("=" * 60)
    
    # 等待用户确认
    input("\n按回车键退出...")

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n程序已取消")
    except Exception as e:
        print(f"\n程序异常: {e}")
        input("\n按回车键退出...")

