################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS/core_riscv.c 

OBJS += \
./RVMSIS/core_riscv.o 

C_DEPS += \
./RVMSIS/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
RVMSIS/core_riscv.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/RVMSIS/core_riscv.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

