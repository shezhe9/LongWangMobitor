################################################################################
# MRS Version: 1.9.2
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_adc.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_clk.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_flash.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_gpio.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_i2c.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_pwm.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_pwr.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_spi0.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_spi1.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_sys.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer0.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer1.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer2.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer3.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart0.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart1.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart2.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart3.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2dev.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2hostBase.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2hostClass.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbdev.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbhostBase.c \
D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbhostClass.c 

OBJS += \
./StdPeriphDriver/CH58x_adc.o \
./StdPeriphDriver/CH58x_clk.o \
./StdPeriphDriver/CH58x_flash.o \
./StdPeriphDriver/CH58x_gpio.o \
./StdPeriphDriver/CH58x_i2c.o \
./StdPeriphDriver/CH58x_pwm.o \
./StdPeriphDriver/CH58x_pwr.o \
./StdPeriphDriver/CH58x_spi0.o \
./StdPeriphDriver/CH58x_spi1.o \
./StdPeriphDriver/CH58x_sys.o \
./StdPeriphDriver/CH58x_timer0.o \
./StdPeriphDriver/CH58x_timer1.o \
./StdPeriphDriver/CH58x_timer2.o \
./StdPeriphDriver/CH58x_timer3.o \
./StdPeriphDriver/CH58x_uart0.o \
./StdPeriphDriver/CH58x_uart1.o \
./StdPeriphDriver/CH58x_uart2.o \
./StdPeriphDriver/CH58x_uart3.o \
./StdPeriphDriver/CH58x_usb2dev.o \
./StdPeriphDriver/CH58x_usb2hostBase.o \
./StdPeriphDriver/CH58x_usb2hostClass.o \
./StdPeriphDriver/CH58x_usbdev.o \
./StdPeriphDriver/CH58x_usbhostBase.o \
./StdPeriphDriver/CH58x_usbhostClass.o 

C_DEPS += \
./StdPeriphDriver/CH58x_adc.d \
./StdPeriphDriver/CH58x_clk.d \
./StdPeriphDriver/CH58x_flash.d \
./StdPeriphDriver/CH58x_gpio.d \
./StdPeriphDriver/CH58x_i2c.d \
./StdPeriphDriver/CH58x_pwm.d \
./StdPeriphDriver/CH58x_pwr.d \
./StdPeriphDriver/CH58x_spi0.d \
./StdPeriphDriver/CH58x_spi1.d \
./StdPeriphDriver/CH58x_sys.d \
./StdPeriphDriver/CH58x_timer0.d \
./StdPeriphDriver/CH58x_timer1.d \
./StdPeriphDriver/CH58x_timer2.d \
./StdPeriphDriver/CH58x_timer3.d \
./StdPeriphDriver/CH58x_uart0.d \
./StdPeriphDriver/CH58x_uart1.d \
./StdPeriphDriver/CH58x_uart2.d \
./StdPeriphDriver/CH58x_uart3.d \
./StdPeriphDriver/CH58x_usb2dev.d \
./StdPeriphDriver/CH58x_usb2hostBase.d \
./StdPeriphDriver/CH58x_usb2hostClass.d \
./StdPeriphDriver/CH58x_usbdev.d \
./StdPeriphDriver/CH58x_usbhostBase.d \
./StdPeriphDriver/CH58x_usbhostClass.d 


# Each subdirectory must supply rules for building sources it contributes
StdPeriphDriver/CH58x_adc.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_adc.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_clk.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_clk.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_flash.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_flash.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_gpio.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_gpio.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_i2c.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_i2c.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_pwm.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_pwm.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_pwr.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_pwr.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_spi0.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_spi0.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_spi1.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_spi1.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_sys.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_sys.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_timer0.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer0.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_timer1.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer1.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_timer2.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer2.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_timer3.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_timer3.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_uart0.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart0.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_uart1.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart1.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_uart2.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart2.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_uart3.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_uart3.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usb2dev.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2dev.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usb2hostBase.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2hostBase.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usb2hostClass.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usb2hostClass.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usbdev.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbdev.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usbhostBase.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbhostBase.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@
StdPeriphDriver/CH58x_usbhostClass.o: D:/BaiduSyncdisk/Tools/Raditor/Code/EXAM_PC/SRC/StdPeriphDriver/CH58x_usbhostClass.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -DCLK_OSC32K=0 -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Startup" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\APP\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HIT_TEST\Profile\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\StdPeriphDriver\inc" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\HAL\include" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\Ld" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\BLE\LIB" -I"D:\BaiduSyncdisk\Tools\Raditor\Code\EXAM_PC\SRC\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

