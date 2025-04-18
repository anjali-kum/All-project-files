################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/authentication.c \
../Core/Src/display.c \
../Core/Src/ds1307_i2c.c \
../Core/Src/fatfs_sd_card.c \
../Core/Src/main.c \
../Core/Src/mem_w25qxxDriver.c \
../Core/Src/rp203ThermalPrinterDriver.c \
../Core/Src/stm32f1xx_hal_msp.c \
../Core/Src/stm32f1xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f1xx.c 

OBJS += \
./Core/Src/authentication.o \
./Core/Src/display.o \
./Core/Src/ds1307_i2c.o \
./Core/Src/fatfs_sd_card.o \
./Core/Src/main.o \
./Core/Src/mem_w25qxxDriver.o \
./Core/Src/rp203ThermalPrinterDriver.o \
./Core/Src/stm32f1xx_hal_msp.o \
./Core/Src/stm32f1xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f1xx.o 

C_DEPS += \
./Core/Src/authentication.d \
./Core/Src/display.d \
./Core/Src/ds1307_i2c.d \
./Core/Src/fatfs_sd_card.d \
./Core/Src/main.d \
./Core/Src/mem_w25qxxDriver.d \
./Core/Src/rp203ThermalPrinterDriver.d \
./Core/Src/stm32f1xx_hal_msp.d \
./Core/Src/stm32f1xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f1xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../FATFS/Target -I../FATFS/App -I../Middlewares/Third_Party/FatFs/src -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/authentication.d ./Core/Src/authentication.o ./Core/Src/authentication.su ./Core/Src/display.d ./Core/Src/display.o ./Core/Src/display.su ./Core/Src/ds1307_i2c.d ./Core/Src/ds1307_i2c.o ./Core/Src/ds1307_i2c.su ./Core/Src/fatfs_sd_card.d ./Core/Src/fatfs_sd_card.o ./Core/Src/fatfs_sd_card.su ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/mem_w25qxxDriver.d ./Core/Src/mem_w25qxxDriver.o ./Core/Src/mem_w25qxxDriver.su ./Core/Src/rp203ThermalPrinterDriver.d ./Core/Src/rp203ThermalPrinterDriver.o ./Core/Src/rp203ThermalPrinterDriver.su ./Core/Src/stm32f1xx_hal_msp.d ./Core/Src/stm32f1xx_hal_msp.o ./Core/Src/stm32f1xx_hal_msp.su ./Core/Src/stm32f1xx_it.d ./Core/Src/stm32f1xx_it.o ./Core/Src/stm32f1xx_it.su ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f1xx.d ./Core/Src/system_stm32f1xx.o ./Core/Src/system_stm32f1xx.su

.PHONY: clean-Core-2f-Src

