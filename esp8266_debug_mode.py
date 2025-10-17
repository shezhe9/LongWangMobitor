#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ESP8266/ESP32 Boot Mode Controller - Debug Version
带有分步调试和串口数据实时显示功能
"""

import serial
import serial.tools.list_ports
import time
import sys
import threading

# 全局变量控制接收线程
stop_thread = False
serial_port = None

def serial_receive_thread(ser):
    """串口接收线程 - 实时显示接收到的数据"""
    global stop_thread
    print("\n" + "=" * 60)
    print("【串口接收线程已启动 - 实时显示接收数据】")
    print("=" * 60 + "\n")
    
    while not stop_thread:
        try:
            if ser.in_waiting > 0:
                # 读取数据
                data = ser.read(ser.in_waiting)
                try:
                    # 尝试解码为字符串
                    text = data.decode('utf-8', errors='ignore')
                    print(f"[接收] {text}", end='', flush=True)
                except:
                    # 如果解码失败，显示十六进制
                    hex_str = ' '.join([f'{b:02X}' for b in data])
                    print(f"[接收HEX] {hex_str}")
            time.sleep(0.01)  # 小延迟避免CPU占用过高
        except Exception as e:
            if not stop_thread:
                print(f"\n[接收线程错误] {e}")
            break
    
    print("\n" + "=" * 60)
    print("【串口接收线程已停止】")
    print("=" * 60 + "\n")

def wait_for_user_confirm(step_description):
    """等待用户确认后继续"""
    print("\n" + "-" * 60)
    print(f">>> {step_description}")
    print("-" * 60)
    input(">>> 按回车键继续下一步...")

def list_serial_ports():
    """列出所有可用的串口"""
    ports = serial.tools.list_ports.comports()
    available_ports = []
    
    print("\n=== 可用串口列表 ===")
    for i, port in enumerate(ports, 1):
        print(f"{i}. {port.device} - {port.description}")
        available_ports.append(port.device)
    
    return available_ports

def debug_boot_mode(port_name):
    """
    调试模式：分步控制 ESP8266/ESP32 进入 Boot 模式
    带有串口数据实时显示功能
    """
    global stop_thread, serial_port
    
    try:
        print(f"\n{'=' * 60}")
        print(f"正在打开串口: {port_name}")
        print(f"波特率: 115200")
        print(f"{'=' * 60}")
        
        # 打开串口
        ser = serial.Serial(port_name, 115200, timeout=0.1)
        serial_port = ser
        print("✓ 串口已打开")
        
        # 启动接收线程
        stop_thread = False
        receive_thread = threading.Thread(target=serial_receive_thread, args=(ser,), daemon=True)
        receive_thread.start()
        
        time.sleep(0.5)  # 等待接收线程启动
        
        print("\n" + "=" * 60)
        print("开始分步调试 ESP Boot 模式时序")
        print("=" * 60)
        print("\n说明：")
        print("- DTR 控制 GPIO0（True=低电平=0V, False=高电平=VCC）")
        print("- RTS 控制 EN/RESET（True=低电平=复位, False=高电平=运行）")
        print("- 每步操作后，请观察串口输出，然后按回车继续\n")
        
        wait_for_user_confirm("准备开始测试，请确认 ESP 设备已连接")
        
        # ========== 步骤 1 ==========
        print("\n" + "=" * 60)
        print("【步骤 1】芯片复位（EN=LOW）")
        print("=" * 60)
        print("操作: DTR=False (DTR引脚=VCC), RTS=True (RTS引脚=0V)")
        print("逻辑: DTR=1, RTS=0 → Q1导通, Q2截止 → EN=0, IO0=1")
        print("预期: 芯片进入复位状态，应该没有任何串口输出")
        print("      EN=0V, IO0=3.3V（或上拉电压）")
        ser.dtr = False  # DTR引脚=VCC → DTR逻辑=1
        ser.rts = True   # RTS引脚=0V → RTS逻辑=0
        print("✓ 已执行: DTR=False(VCC), RTS=True(0V) → EN=0, IO0=1")
        print("延时 100ms，让 EN 电容完全放电...")
        time.sleep(0.1)
        
        wait_for_user_confirm("观察: 芯片应该在复位状态，EN应为0V，IO0应为高电平")
        
        # ========== 步骤 2 ==========
        print("\n" + "=" * 60)
        print("【步骤 2】释放 RTS（EN 开始上升）")
        print("=" * 60)
        print("操作: DTR=True (DTR引脚=0V), RTS=True (RTS引脚=0V)")
        print("逻辑: DTR=0, RTS=0 → Q1截止, Q2截止 → EN=1, IO0=1")
        print("预期: EN 由于 RC 电路，会缓慢上升（不是立即变高）")
        print("      IO0 仍为高电平")
        ser.dtr = True   # DTR引脚=0V → DTR逻辑=0
        # RTS 保持 True (0V)
        print("✓ 已执行: DTR=True(0V), RTS=True(0V) → EN开始上升, IO0=1")
        
        wait_for_user_confirm("观察: EN 应该开始从 0V 缓慢上升，IO0 应为高电平")
        
        # ========== 步骤 3 ==========
        print("\n" + "=" * 60)
        print("【步骤 3】拉低 IO0（关键步骤）")
        print("=" * 60)
        print("操作: DTR=True (保持0V), RTS=False (RTS引脚=VCC)")
        print("逻辑: DTR=0, RTS=1 → Q1截止, Q2导通 → EN=1, IO0=0")
        print("预期: EN 继续上升，IO0 被拉低")
        print("      当 EN 上升到阈值时，IO0 为低，芯片进入 Boot 模式！")
        ser.rts = False  # RTS引脚=VCC → RTS逻辑=1
        # DTR 保持 True (0V)
        print("✓ 已执行: DTR=True(0V), RTS=False(VCC) → EN=1(上升中), IO0=0")
        print("延时 50ms，让 EN 完全上升到高电平...")
        time.sleep(0.05)
        
        wait_for_user_confirm("观察: 串口应该输出 Boot 模式信息（waiting for download）")
        
        # ========== 步骤 4 ==========
        print("\n" + "=" * 60)
        print("【步骤 4】恢复 IO0 为高（可选）")
        print("=" * 60)
        print("操作: DTR=False (DTR引脚=VCC), RTS=False (RTS引脚=VCC)")
        print("逻辑: DTR=1, RTS=1 → Q1截止, Q2截止 → EN=1, IO0=1")
        print("预期: 芯片应该继续保持在 Boot 模式")
        ser.dtr = False  # DTR引脚=VCC → DTR逻辑=1
        # RTS 保持 False (VCC)
        print("✓ 已执行: DTR=False(VCC), RTS=False(VCC) → EN=1, IO0=1")
        
        wait_for_user_confirm("GPIO0 恢复为高，芯片应该仍在 Boot 模式")
        
        # ========== 完成 ==========
        print("\n" + "=" * 60)
        print("【完成】Boot 模式测试序列完成")
        print("=" * 60)
        print("\n现在您可以：")
        print("1. 使用烧录工具进行固件下载")
        print("2. 按回车键关闭串口并退出")
        print("3. 或者继续观察串口输出")
        
        input("\n按回车键退出...")
        
        # 停止接收线程并关闭串口
        print("\n正在关闭...")
        stop_thread = True
        time.sleep(0.2)  # 等待接收线程结束
        ser.close()
        print("✓ 串口已关闭")
        
        return True
        
    except serial.SerialException as e:
        print(f"\n✗ 串口错误: {e}")
        print("请确保：")
        print("1. 串口未被其他程序占用")
        print("2. USB 驱动已正确安装")
        print("3. 设备已正确连接")
        return False
    except KeyboardInterrupt:
        print("\n\n用户中断")
        stop_thread = True
        if serial_port:
            serial_port.close()
        return False
    except Exception as e:
        print(f"\n✗ 发生错误: {e}")
        stop_thread = True
        if serial_port:
            serial_port.close()
        return False

def main():
    print("=" * 60)
    print("ESP8266/ESP32 Boot Mode Controller - Debug Version")
    print("ESP 启动模式控制工具 - 调试版本")
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
    
    # 执行调试模式
    debug_boot_mode(selected_port)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\n\n程序已取消")
        stop_thread = True
    except Exception as e:
        print(f"\n程序异常: {e}")
        import traceback
        traceback.print_exc()
        input("\n按回车键退出...")

