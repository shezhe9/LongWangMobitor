################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include HAL/subdir.mk
-include APP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
Central_连接HIDKBD.hex \

SECONDARY_LIST += \
Central_连接HIDKBD.lst \

SECONDARY_SIZE += \
Central_连接HIDKBD.siz \


# All Target
all: Central_连接HIDKBD.elf secondary-outputs

# Tool invocations
Central_连接HIDKBD.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -T "D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\BLE\LIB" -L"D:\BaiduSyncdisk\Tools\Raditor\Code\CH583EVT\EXAM\SRC\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"Central_连接HIDKBD.map" --specs=nano.specs --specs=nosys.specs -o "Central_连接HIDKBD.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
Central_连接HIDKBD.hex: Central_连接HIDKBD.elf
	@	riscv-none-embed-objcopy -O ihex "Central_连接HIDKBD.elf"  "Central_连接HIDKBD.hex"
	@	@
Central_连接HIDKBD.lst: Central_连接HIDKBD.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "Central_连接HIDKBD.elf" > "Central_连接HIDKBD.lst"
	@	@
Central_连接HIDKBD.siz: Central_连接HIDKBD.elf
	@	riscv-none-embed-size --format=berkeley "Central_连接HIDKBD.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) Central_连接HIDKBD.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
