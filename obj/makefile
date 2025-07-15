################################################################################
# MRS Version: 2.2.0
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
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
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
all: 
	$(MAKE) --no-print-directory main-build 

main-build: HID-TEST.elf secondary-outputs

# Tool invocations
HID-TEST.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -T "e:/Code/EXAM/SRC/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"e:\Code\EXAM\BLE\ch582test" -L"e:/Code/EXAM/BLE/LIB" -L"e:/Code/EXAM/SRC/StdPeriphDriver" -Xlinker --print-memory-usage -Xlinker --print-memory-usage -Wl,-Map,"HID-TEST.map" --specs=nano.specs --specs=nosys.specs -o "HID-TEST.elf" $(OBJS)  $(LIBS)
HID-TEST.hex: HID-TEST.elf
	@	riscv-none-embed-objcopy -O ihex "HID-TEST.elf" "HID-TEST.hex"
HID-TEST.lst: HID-TEST.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "HID-TEST.elf" > "HID-TEST.lst"
HID-TEST.siz: HID-TEST.elf
	riscv-none-embed-size --format=berkeley "HID-TEST.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) HID-TEST.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets