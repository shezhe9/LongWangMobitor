################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
e:/Code/EXAM/BLE/HAL/MCU.c \
e:/Code/EXAM/BLE/HAL/RTC.c \
e:/Code/EXAM/BLE/HAL/SLEEP.c 

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
HAL/MCU.o: e:/Code/EXAM/BLE/HAL/MCU.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"e:/Code/EXAM/SRC/Startup" -I"e:/Code/EXAM/BLE/ch582test/APP/include" -I"e:/Code/EXAM/BLE/ch582test/Profile/include" -I"e:/Code/EXAM/SRC/StdPeriphDriver/inc" -I"e:/Code/EXAM/BLE/HAL/include" -I"e:/Code/EXAM/SRC/Ld" -I"e:/Code/EXAM/BLE/LIB" -I"e:/Code/EXAM/SRC/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
HAL/RTC.o: e:/Code/EXAM/BLE/HAL/RTC.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"e:/Code/EXAM/SRC/Startup" -I"e:/Code/EXAM/BLE/ch582test/APP/include" -I"e:/Code/EXAM/BLE/ch582test/Profile/include" -I"e:/Code/EXAM/SRC/StdPeriphDriver/inc" -I"e:/Code/EXAM/BLE/HAL/include" -I"e:/Code/EXAM/SRC/Ld" -I"e:/Code/EXAM/BLE/LIB" -I"e:/Code/EXAM/SRC/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
HAL/SLEEP.o: e:/Code/EXAM/BLE/HAL/SLEEP.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"e:/Code/EXAM/SRC/Startup" -I"e:/Code/EXAM/BLE/ch582test/APP/include" -I"e:/Code/EXAM/BLE/ch582test/Profile/include" -I"e:/Code/EXAM/SRC/StdPeriphDriver/inc" -I"e:/Code/EXAM/BLE/HAL/include" -I"e:/Code/EXAM/SRC/Ld" -I"e:/Code/EXAM/BLE/LIB" -I"e:/Code/EXAM/SRC/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

