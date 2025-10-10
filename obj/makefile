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
LongWangMonitor.hex \

SECONDARY_LIST += \
LongWangMonitor.lst \

SECONDARY_SIZE += \
LongWangMonitor.siz \


# All Target
all: 
	$(MAKE) --no-print-directory main-build 

main-build: LongWangMonitor.elf secondary-outputs

# Tool invocations
LongWangMonitor.elf: $(OBJS) $(USER_OBJS)
	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -fmax-errors=20 -Os -fsigned-char -ffunction-sections -fno-common -g -T "d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/Ld/Link.ld" -nostartfiles -Xlinker --gc-sections -L"d:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LongWangMonitor" -L"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/BLE/LIB" -L"d:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver" -Xlinker --print-memory-usage -Xlinker --print-memory-usage -Wl,-Map,"LongWangMonitor.map" --specs=nano.specs --specs=nosys.specs -o "LongWangMonitor.elf" $(OBJS)  $(LIBS)
LongWangMonitor.hex: LongWangMonitor.elf
	@	riscv-none-embed-objcopy -O ihex "LongWangMonitor.elf" "LongWangMonitor.hex"
LongWangMonitor.lst: LongWangMonitor.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "LongWangMonitor.elf" > "LongWangMonitor.lst"
LongWangMonitor.siz: LongWangMonitor.elf
	riscv-none-embed-size --format=berkeley "LongWangMonitor.elf"

# Other Targets
clean:
	-$(RM) $(OBJS) $(EXPANDS) $(CALLGRAPH_DOT) $(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(S_DEPS)$(S_UPPER_DEPS)$(ASM_DEPS)$(ASM_UPPER_DEPS)$(C_DEPS) LongWangMonitor.elf

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets