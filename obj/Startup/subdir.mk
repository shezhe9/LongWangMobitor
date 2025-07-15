################################################################################
# MRS Version: 2.2.0
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_UPPER_SRCS += \
e:/Code/EXAM/SRC/Startup/startup_CH583.S 

S_UPPER_DEPS += \
./Startup/startup_CH583.d 

OBJS += \
./Startup/startup_CH583.o 


EXPANDS += \
./Startup/startup_CH583.S.234r.expand 



# Each subdirectory must supply rules for building sources it contributes
Startup/startup_CH583.o: e:/Code/EXAM/SRC/Startup/startup_CH583.S
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -x assembler -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"

