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
HID-TEST.hex \

SECONDARY_LIST += \
HID-TEST.lst \

SECONDARY_SIZE += \
HID-TEST.siz \


# All Target
all: HID-TEST.elf secondary-outputs

# Tool invocations
HID-TEST.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -T "D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -L"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"HID-TEST.map" --specs=nano.specs --specs=nosys.specs -o "HID-TEST.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
HID-TEST.hex: HID-TEST.elf
	@	riscv-none-embed-objcopy -O ihex "HID-TEST.elf"  "HID-TEST.hex"
	@	@
HID-TEST.lst: HID-TEST.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "HID-TEST.elf" > "HID-TEST.lst"
	@	@
HID-TEST.siz: HID-TEST.elf
	@	riscv-none-embed-size --format=berkeley "HID-TEST.elf"
	@	@
# Other Targets
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) HID-TEST.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
