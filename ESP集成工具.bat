@echo off
chcp 65001 >nul
title ESP8266/ESP32 集成控制工具

echo ========================================
echo ESP 集成控制工具
echo 串口监控 + Boot 模式控制
echo ========================================
echo.

REM 检查 Python 是否安装
python --version >nul 2>&1
if %errorlevel% neq 0 (
    echo [错误] 未检测到 Python！
    echo.
    echo 请先安装 Python 3.x:
    echo https://www.python.org/downloads/
    echo.
    pause
    exit /b 1
)

echo [✓] Python 已安装
echo.

REM 检查 pyserial 是否安装
python -c "import serial" >nul 2>&1
if %errorlevel% neq 0 (
    echo [提示] 未检测到 pyserial 库，正在自动安装...
    echo.
    pip install pyserial
    if %errorlevel% neq 0 (
        echo.
        echo [错误] pyserial 安装失败！
        pause
        exit /b 1
    )
    echo.
    echo [✓] pyserial 安装成功
    echo.
) else (
    echo [✓] pyserial 已安装
    echo.
)

REM 运行集成工具
python "%~dp0esp8266_integrated_tool.py"

exit /b 0

