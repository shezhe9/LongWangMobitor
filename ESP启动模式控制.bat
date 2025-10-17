@echo off
chcp 65001 >nul
title ESP8266/ESP32 启动模式控制工具

echo ========================================
echo ESP8266/ESP32 Boot Mode Controller
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
    echo 安装时请勾选 "Add Python to PATH"
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
        echo 请手动运行: pip install pyserial
        echo.
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

REM 运行 Python 脚本
echo ========================================
echo 启动控制工具...
echo ========================================
echo.

python "%~dp0esp8266_boot_mode.py"

if %errorlevel% neq 0 (
    echo.
    echo [错误] 程序运行出错
    pause
    exit /b 1
)

exit /b 0

