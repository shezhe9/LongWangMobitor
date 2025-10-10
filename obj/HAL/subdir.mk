################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/MCU.c \
d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/RTC.c \
d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/SLEEP.c 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 


EXPANDS += \
./HAL/MCU.c.234r.expand \
./HAL/RTC.c.234r.expand \
./HAL/SLEEP.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
HAL/MCU.o: d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/MCU.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Startup" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/APP/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Profile/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/inc" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LIB" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
HAL/RTC.o: d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/RTC.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Startup" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/APP/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Profile/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/inc" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LIB" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
HAL/SLEEP.o: d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/SLEEP.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Startup" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/APP/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Profile/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/inc" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/HAL/include" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LIB" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog" -I"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LongWangMonitor/Driver/ulog/inc" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

