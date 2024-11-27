################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/Data_Reception/app_data_reception.c 

OBJS += \
./APP/Data_Reception/app_data_reception.o 

C_DEPS += \
./APP/Data_Reception/app_data_reception.d 


# Each subdirectory must supply rules for building sources it contributes
APP/Data_Reception/%.o APP/Data_Reception/%.su APP/Data_Reception/%.cyclo: ../APP/Data_Reception/%.c APP/Data_Reception/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/ADC -I../BSP/UART -I../BSP/GPIO -I../Middleware/Ring_Buffer -I../BSP/DMA -I../BSP/TIMER -I../APP/Common -I../Scheduler -I../APP/Calculator_PF -I../APP/Read_Peek_Voltage_Current -I../BSP/SYSTICK -I../APP/Data -I../APP/Data_Transmission -I../APP/CMDLine/Include -I../Device/ACS712 -I../APP/Led_7_Seg -I../APP/Data_Reception -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APP-2f-Data_Reception

clean-APP-2f-Data_Reception:
	-$(RM) ./APP/Data_Reception/app_data_reception.cyclo ./APP/Data_Reception/app_data_reception.d ./APP/Data_Reception/app_data_reception.o ./APP/Data_Reception/app_data_reception.su

.PHONY: clean-APP-2f-Data_Reception

