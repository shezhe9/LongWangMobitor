#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
ESP8266/ESP32 集成控制工具
功能：串口监控 + Boot 模式控制 + DTR/RTS 控制

一个工具完成所有功能，无需额外的串口调试软件
"""

import serial
import serial.tools.list_ports
import time
import sys
import threading
import msvcrt  # Windows 键盘输入

# 全局变量
stop_thread = False
serial_port = None
menu_mode = False  # 是否在菜单模式

def clear_screen():
    """清屏"""
    import os
    os.system('cls' if os.name == 'nt' else 'clear')

def serial_receive_thread(ser):
    """串口接收线程 - 实时显示接收到的数据"""
    global stop_thread
    
    while not stop_thread:
        try:
            if ser and ser.is_open and ser.in_waiting > 0:
                # 读取数据
                data = ser.read(ser.in_waiting)
                try:
                    # 尝试解码为字符串
                    text = data.decode('utf-8', errors='ignore')
                    if not menu_mode:  # 只在非菜单模式下显示
                        print(f"{text}", end='', flush=True)
                except:
                    # 如果解码失败，显示十六进制
                    hex_str = ' '.join([f'{b:02X}' for b in data])
                    if not menu_mode:
                        print(f"[HEX] {hex_str}")
            time.sleep(0.01)
        except Exception as e:
            if not stop_thread:
                print(f"\n[接收线程错误] {e}")
            break

def list_serial_ports():
    """列出所有可用的串口"""
    ports = serial.tools.list_ports.comports()
    available_ports = []
    
    print("\n=== 可用串口列表 ===")
    for i, port in enumerate(ports, 1):
        print(f"{i}. {port.device} - {port.description}")
        available_ports.append(port.device)
    
    return available_ports

def enter_boot_mode(ser):
    """进入 Boot 模式（自动执行完整时序）"""
    print("\n" + "=" * 60)
    print("【进入 Boot 模式】")
    print("=" * 60)
    
    try:
        print("\n执行 ESP8266 自动下载时序...")
        
        # 步骤 1: 芯片复位
        print("步骤 1: 芯片复位 (EN=0, IO0=1)")
        ser.dtr = False
        ser.rts = True
        time.sleep(0.1)
        
        # 步骤 2: EN 开始上升
        print("步骤 2: EN 开始上升 (IO0=1)")
        ser.dtr = True
        time.sleep(0.001)  # 很短的延时
        
        # 步骤 3: 拉低 IO0（关键）
        print("步骤 3: 拉低 IO0 (EN上升中, IO0=0)")
        ser.rts = False
        time.sleep(0.05)
        
        # 步骤 4: 恢复 IO0
        print("步骤 4: 恢复 IO0 (完成)")
        ser.dtr = False
        time.sleep(0.05)
        
        print("\n✓ Boot 模式时序已执行")
        print("观察串口输出，应该看到 'waiting for download'")
        
    except Exception as e:
        print(f"\n✗ 错误: {e}")

def reset_to_normal(ser):
    """复位到正常运行模式"""
    print("\n" + "=" * 60)
    print("【复位到正常运行模式】")
    print("=" * 60)
    
    try:
        print("执行复位...")
        ser.dtr = False  # IO0=HIGH (正常启动)
        ser.rts = True   # EN=LOW (复位)
        time.sleep(0.1)
        ser.rts = False  # EN=HIGH (启动)
        time.sleep(0.05)
        
        print("✓ 已复位到正常运行模式")
        
    except Exception as e:
        print(f"\n✗ 错误: {e}")

def manual_control_dtr_rts(ser):
    """手动控制 DTR 和 RTS"""
    print("\n" + "=" * 60)
    print("【手动控制 DTR/RTS】")
    print("=" * 60)
    print("\n说明：")
    print("  DTR: False=VCC(高), True=0V(低)")
    print("  RTS: False=VCC(高), True=0V(低)")
    print("\n当前状态：")
    print(f"  DTR = {ser.dtr}")
    print(f"  RTS = {ser.rts}")
    
    while True:
        print("\n" + "-" * 60)
        print("1. 设置 DTR=False(VCC)    2. 设置 DTR=True(0V)")
        print("3. 设置 RTS=False(VCC)    4. 设置 RTS=True(0V)")
        print("0. 返回主菜单")
        print("-" * 60)
        
        choice = input("选择操作: ").strip()
        
        if choice == '1':
            ser.dtr = False
            print("✓ DTR = False (VCC)")
        elif choice == '2':
            ser.dtr = True
            print("✓ DTR = True (0V)")
        elif choice == '3':
            ser.rts = False
            print("✓ RTS = False (VCC)")
        elif choice == '4':
            ser.rts = True
            print("✓ RTS = True (0V)")
        elif choice == '0':
            break
        else:
            print("无效选择")
        
        print(f"\n当前状态: DTR={ser.dtr}, RTS={ser.rts}")

def send_data(ser):
    """发送数据到串口"""
    print("\n" + "=" * 60)
    print("【发送数据】")
    print("=" * 60)
    print("提示：输入 'exit' 返回主菜单\n")
    
    while True:
        try:
            data = input("发送 > ")
            if data.lower() == 'exit':
                break
            
            # 添加换行符
            data += '\r\n'
            ser.write(data.encode('utf-8'))
            print(f"✓ 已发送: {repr(data)}")
            
        except Exception as e:
            print(f"✗ 发送错误: {e}")

def show_help():
    """显示帮助信息"""
    print("\n" + "=" * 60)
    print("【使用说明】")
    print("=" * 60)
    print("""
ESP8266/ESP32 自动下载电路原理：
- DTR 通过三极管控制 GPIO0
- RTS 通过三极管控制 EN/RESET
- EN 连接 RC 电路，复位/启动有延时

进入 Boot 模式的条件：
- EN 上升沿时，GPIO0 为低电平

工具功能：
1. 自动进入 Boot 模式：执行完整的自动下载时序
2. 复位到正常模式：复位芯片，正常启动固件
3. 手动控制：精确控制 DTR/RTS 信号
4. 发送数据：向串口发送命令或数据
5. 实时监控：后台持续显示串口接收数据

注意事项：
- 串口保持打开状态，可随时操作
- 串口数据实时显示，无需额外工具
- 使用此工具时，请关闭其他串口软件
""")
    input("\n按回车键返回...")

def main_menu(ser):
    """主菜单"""
    global menu_mode
    
    while True:
        menu_mode = True
        clear_screen()
        
        print("=" * 60)
        print("ESP8266/ESP32 集成控制工具")
        print("=" * 60)
        print(f"\n当前串口: {ser.port}")
        print(f"波特率: {ser.baudrate}")
        print(f"状态: {'已连接' if ser.is_open else '已断开'}")
        print(f"\nDTR={ser.dtr}, RTS={ser.rts}")
        
        print("\n" + "-" * 60)
        print("【主菜单】")
        print("-" * 60)
        print("1. 进入 Boot 模式（自动执行完整时序）")
        print("2. 复位到正常运行模式")
        print("3. 手动控制 DTR/RTS 信号")
        print("4. 发送数据到串口")
        print("5. 查看帮助信息")
        print("6. 切换到监控模式（实时查看串口数据）")
        print("0. 退出程序")
        print("-" * 60)
        
        choice = input("\n请选择操作 (0-6): ").strip()
        
        if choice == '1':
            enter_boot_mode(ser)
            input("\n按回车键返回主菜单...")
        elif choice == '2':
            reset_to_normal(ser)
            input("\n按回车键返回主菜单...")
        elif choice == '3':
            manual_control_dtr_rts(ser)
        elif choice == '4':
            send_data(ser)
        elif choice == '5':
            show_help()
        elif choice == '6':
            monitor_mode(ser)
        elif choice == '0':
            print("\n正在退出...")
            return
        else:
            print("无效选择，请重试")
            time.sleep(1)

def monitor_mode(ser):
    """监控模式 - 全屏显示串口数据"""
    global menu_mode
    menu_mode = False
    
    clear_screen()
    print("=" * 60)
    print("串口监控模式 - 实时显示串口数据")
    print("=" * 60)
    print("按 Ctrl+C 或 ESC 返回主菜单\n")
    print("-" * 60 + "\n")
    
    try:
        while True:
            # 检查键盘输入（Windows）
            if msvcrt.kbhit():
                key = msvcrt.getch()
                if key == b'\x1b':  # ESC
                    break
                elif key == b'\x03':  # Ctrl+C
                    break
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    
    print("\n\n" + "-" * 60)
    print("返回主菜单...")
    time.sleep(1)

def main():
    global stop_thread, serial_port
    
    print("=" * 60)
    print("ESP8266/ESP32 集成控制工具")
    print("串口监控 + Boot 模式控制")
    print("=" * 60)
    
    # 列出可用串口
    ports = list_serial_ports()
    
    if not ports:
        print("\n✗ 未检测到可用串口！")
        input("\n按回车键退出...")
        return
    
    # 用户选择串口
    print("\n" + "=" * 60)
    while True:
        try:
            choice = input("请输入串口编号 (如 1): ").strip()
            port_index = int(choice) - 1
            if 0 <= port_index < len(ports):
                selected_port = ports[port_index]
                break
            else:
                print(f"✗ 无效选择，请输入 1-{len(ports)} 之间的数字")
        except ValueError:
            print("✗ 请输入有效的数字")
    
    # 选择波特率
    print("\n常用波特率:")
    print("1. 115200 (推荐)")
    print("2. 74880 (ESP8266 Boot 默认)")
    print("3. 9600")
    print("4. 自定义")
    
    baud_choice = input("\n选择波特率 (默认 1): ").strip() or '1'
    
    baud_map = {
        '1': 115200,
        '2': 74880,
        '3': 9600
    }
    
    if baud_choice in baud_map:
        baudrate = baud_map[baud_choice]
    elif baud_choice == '4':
        baudrate = int(input("输入自定义波特率: "))
    else:
        baudrate = 115200
    
    # 打开串口
    try:
        print(f"\n正在打开串口: {selected_port} @ {baudrate}...")
        ser = serial.Serial(selected_port, baudrate, timeout=0.1)
        serial_port = ser
        print("✓ 串口已打开")
        
        # 启动接收线程
        stop_thread = False
        receive_thread = threading.Thread(target=serial_receive_thread, args=(ser,), daemon=True)
        receive_thread.start()
        print("✓ 接收线程已启动")
        
        time.sleep(0.5)
        
        # 进入主菜单
        main_menu(ser)
        
        # 清理
        print("\n正在关闭...")
        stop_thread = True
        time.sleep(0.2)
        ser.close()
        print("✓ 串口已关闭")
        print("\n感谢使用！")
        
    except serial.SerialException as e:
        print(f"\n✗ 串口错误: {e}")
        print("请确保：")
        print("1. 串口未被其他程序占用")
        print("2. USB 驱动已正确安装")
        print("3. 设备已正确连接")
        input("\n按回车键退出...")
    except KeyboardInterrupt:
        print("\n\n用户中断")
        stop_thread = True
        if serial_port:
            serial_port.close()
    except Exception as e:
        print(f"\n✗ 发生错误: {e}")
        import traceback
        traceback.print_exc()
        input("\n按回车键退出...")

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

