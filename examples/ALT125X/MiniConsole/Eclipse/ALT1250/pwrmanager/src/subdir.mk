################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/fastlz.c \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/hibernate.c \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/mem_retain.c \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/pwr_mngr.c \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/sleep_mngr.c \
C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/sleep_notify.c 

OBJS += \
./pwrmanager/src/fastlz.o \
./pwrmanager/src/hibernate.o \
./pwrmanager/src/mem_retain.o \
./pwrmanager/src/pwr_mngr.o \
./pwrmanager/src/sleep_mngr.o \
./pwrmanager/src/sleep_notify.o 

C_DEPS += \
./pwrmanager/src/fastlz.d \
./pwrmanager/src/hibernate.d \
./pwrmanager/src/mem_retain.d \
./pwrmanager/src/pwr_mngr.d \
./pwrmanager/src/sleep_mngr.d \
./pwrmanager/src/sleep_notify.d 


# Each subdirectory must supply rules for building sources it contributes
pwrmanager/src/fastlz.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/fastlz.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwrmanager/src/hibernate.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/hibernate.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwrmanager/src/mem_retain.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/mem_retain.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwrmanager/src/pwr_mngr.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/pwr_mngr.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwrmanager/src/sleep_mngr.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/sleep_mngr.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

pwrmanager/src/sleep_notify.o: C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/middleware/pwrmanager/src/sleep_notify.c pwrmanager/src/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU ARM Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -Og -ffunction-sections -fdata-sections -Wall -Wextra -Wno-unused-parameter -g -DALT1250 -DDEVICE_HEADER=\"ALT1250.h\" -DCMSIS_device_header=\"ALT1250.h\" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source" -I"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Chipset\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\Core\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\CMSIS\RTOS2\FreeRTOS\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\FreeRTOS\lib\FreeRTOS\portable\GCC\ARM_CM3" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Include\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\ALT125x\Driver\Source\Private\ALT1250" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\osal\Include" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\core" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\hifc\src\lib\mi" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\serial" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\miniconsole" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\applib" -I"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\pwrmanager\inc" -include"C:/Users/shashank.hegde/Desktop/Adrastea/MCU_02_02_10_00_31121/examples/ALT125X/MiniConsole/Eclipse/../source/config.h" -include"C:\Users\shashank.hegde\Desktop\Adrastea\MCU_02_02_10_00_31121\middleware\printf\printf.h" -std=gnu11 -funwind-tables $(EXTRA_CFLAGS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


