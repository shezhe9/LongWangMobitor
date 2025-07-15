################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
e:/Code/EXAM/SRC/RVMSIS/core_riscv.c 

C_DEPS += \
./RVMSIS/core_riscv.d 

OBJS += \
./RVMSIS/core_riscv.o 


EXPANDS += \
./RVMSIS/core_riscv.c.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
RVMSIS/core_riscv.o: e:/Code/EXAM/SRC/RVMSIS/core_riscv.c
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -DDEBUG=1 -DCLK_OSC32K=0 -I"e:/Code/EXAM/SRC/Startup" -I"e:/Code/EXAM/BLE/ch582test/APP/include" -I"e:/Code/EXAM/BLE/ch582test/Profile/include" -I"e:/Code/EXAM/SRC/StdPeriphDriver/inc" -I"e:/Code/EXAM/BLE/HAL/include" -I"e:/Code/EXAM/SRC/Ld" -I"e:/Code/EXAM/BLE/LIB" -I"e:/Code/EXAM/SRC/RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

