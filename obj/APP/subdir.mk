################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/central.c \
../APP/central_main.c \
../APP/key.c \
../APP/uart_cmd.c \
../APP/ws2812.c 

OBJS += \
./APP/central.o \
./APP/central_main.o \
./APP/key.o \
./APP/uart_cmd.o \
./APP/ws2812.o 

C_DEPS += \
./APP/central.d \
./APP/central_main.d \
./APP/key.d \
./APP/uart_cmd.d \
./APP/ws2812.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

