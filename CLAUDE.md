# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a BLE (Bluetooth Low Energy) Central device project based on the WCH CH582M microcontroller. The CH582M acts as the BLE Central that can discover and connect to specified BLE peripheral devices. The project includes OLED display functionality, UART communication, key handling, and ESP8266/ESP32 boot control utilities.

## Build System

This project uses Eclipse CDT with a GNU MCU RISC-V GCC toolchain.

### Build Commands

**Primary Build:**
```bash
# Navigate to build directory
cd obj

# Build the project
make

# Clean build artifacts
make clean

# Build specific outputs (generated automatically)
# - LongWangMonitor.elf (main executable)
# - LongWangMonitor.hex (for flashing)
# - LongWangMonitor.lst (listing file)
# - LongWangMonitor.siz (size information)
```

**Toolchain:**
- RISC-V GCC: `riscv-none-embed-gcc`
- Architecture: RV32IMAC with compressed extension (RVC)
- ABI: ilp32
- Optimization: -Os (size optimization)
- Linker script: `d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld/Link.ld`

## Project Architecture

### Core Components

**Application Layer (`APP/`):**
- `central_main.c` - Main program entry point, system initialization
- `central.c/central.h` - BLE Central core functionality and event handling
- `uart_cmd.c/uart_cmd.h` - UART command processing and communication
- `key.c/key.h` - Key control module with state machine implementation
- `cmd_parsing.c/cmd_parsing.h` - Command parsing module for various protocols

**Driver Layer (`Driver/`):**
- `i2c_oled/` - OLED display driver (SH1106/SH1107 support)
- `i2c_humi/` - I2C humidity sensor driver
- `ulog/` - Unified logging system with buffered output

**Hardware Abstraction:**
- HAL layer for CH582M hardware abstraction
- RVMSIS for RISC-V core functionality
- StdPeriphDriver for standard peripheral functions

### Key Features

**BLE Functionality:**
- Central role with device discovery and connection
- GATT service discovery and characteristic operations
- Auto-reconnect functionality with configurable timeout
- Connection management with state machine
- RSSI monitoring and link parameter updates

**Display System:**
- OLED support with selectable drivers (SH1106 128x64 or SH1107 64x128)
- Temperature and system status display
- Configurable via `ENABLE_OLED_DISPLAY` define

**Control Interfaces:**
- Multi-key input with state machine handling
- UART command interface for external control
- ESP8266/ESP32 boot mode control via Python scripts

### Configuration

**Missing CONFIG.h:** The project references `CONFIG.h` but this file appears to be located in a parent directory outside the current repository. This file contains global configuration settings for the BLE stack and hardware configuration.

**Key Configuration Areas:**
- BLE stack parameters in `central.h`
- OLED driver selection in `oled_display.h`
- Logging configuration in `ulog_config.h`

## Development Workflow

### ESP Tools (Python Scripts)
Several Python scripts are provided for ESP8266/ESP32 interaction:
- `esp8266_integrated_tool.py` - Main integrated control tool
- `esp8266_debug_mode.py` - Debug mode operations
- `esp8266_boot_mode.py` - Boot mode control
- Batch files for easy execution with dependency checking

### Code Organization Notes
- Chinese comments and documentation throughout the codebase
- Event-driven architecture using TMOS system
- State machines for key handling and connection management
- Modular design with clear separation between application and driver layers

### Important Files to Understand
- `APP/central.h:25-48` - BLE event definitions and constants
- `APP/central_main.c:13` - System initialization includes
- `Driver/i2c_oled/inc/oled_display.h:22-30` - OLED configuration options
- `obj/makefile:60-67` - Build output generation commands