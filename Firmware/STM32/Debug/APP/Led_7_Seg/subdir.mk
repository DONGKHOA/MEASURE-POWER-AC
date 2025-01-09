################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/Led_7_Seg/app_led_7seg.c 

OBJS += \
./APP/Led_7_Seg/app_led_7seg.o 

C_DEPS += \
./APP/Led_7_Seg/app_led_7seg.d 


# Each subdirectory must supply rules for building sources it contributes
APP/Led_7_Seg/%.o APP/Led_7_Seg/%.su APP/Led_7_Seg/%.cyclo: ../APP/Led_7_Seg/%.c APP/Led_7_Seg/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/ADC -I../BSP/UART -I../BSP/GPIO -I../Middleware/Ring_Buffer -I../BSP/DMA -I../BSP/TIMER -I../APP/Common -I../Scheduler -I../APP/Calculator_PF -I../APP/Read_Peek_Voltage_Current -I../BSP/SYSTICK -I../APP/Data -I../APP/CMDLine/Include -I../Device/ACS712 -I../APP/Led_7_Seg -I../APP/Data_Trans_Rec -I../APP/Status_Led -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APP-2f-Led_7_Seg

clean-APP-2f-Led_7_Seg:
	-$(RM) ./APP/Led_7_Seg/app_led_7seg.cyclo ./APP/Led_7_Seg/app_led_7seg.d ./APP/Led_7_Seg/app_led_7seg.o ./APP/Led_7_Seg/app_led_7seg.su

.PHONY: clean-APP-2f-Led_7_Seg

