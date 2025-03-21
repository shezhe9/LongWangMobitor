################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/MCU.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/RTC.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/SLEEP.c 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/MCU.o: D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/MCU.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/RTC.o: D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/RTC.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
HAL/SLEEP.o: D:/BaiduSyncdisk/Tools/Raditor/Code/CH583EVT/EXAM/BLE/HAL/SLEEP.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\Central_连接HIDKBD\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

