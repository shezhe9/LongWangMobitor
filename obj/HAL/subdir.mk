################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/MCU.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/RTC.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/SLEEP.c 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/MCU.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/MCU.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/RTC.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/RTC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/SLEEP.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/SLEEP.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

