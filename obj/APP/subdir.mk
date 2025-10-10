################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/central.c \
../APP/central_main.c \
../APP/key.c \
../APP/uart_cmd.c \
../APP/ws2812.c 

C_DEPS += \
./APP/central.d \
./APP/central_main.d \
./APP/key.d \
./APP/uart_cmd.d \
./APP/ws2812.d 

OBJS += \
./APP/central.o \
./APP/central_main.o \
./APP/key.o \
./APP/uart_cmd.o \
./APP/ws2812.o 


EXPANDS += \
./APP/central.c.234r.expand \
./APP/central_main.c.234r.expand \
./APP/key.c.234r.expand \
./APP/uart_cmd.c.234r.expand \
./APP/ws2812.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Startup" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/APP/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Profile/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/inc" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LIB" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

