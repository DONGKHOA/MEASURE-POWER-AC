################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/Calculator_PF/app_calculator_PF.c 

OBJS += \
./APP/Calculator_PF/app_calculator_PF.o 

C_DEPS += \
./APP/Calculator_PF/app_calculator_PF.d 


# Each subdirectory must supply rules for building sources it contributes
APP/Calculator_PF/%.o APP/Calculator_PF/%.su APP/Calculator_PF/%.cyclo: ../APP/Calculator_PF/%.c APP/Calculator_PF/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/ADC -I../BSP/UART -I../BSP/GPIO -I../Middleware/Ring_Buffer -I../BSP/DMA -I../BSP/TIMER -I../APP/Common -I../Scheduler -I../APP/Calculator_PF -I../APP/Read_Peek_Voltage_Current -I../BSP/SYSTICK -I../APP/Data -I../APP/CMDLine/Include -I../Device/ACS712 -I../APP/Led_7_Seg -I../APP/Data_Trans_Rec -I../APP/Status_Led -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APP-2f-Calculator_PF

clean-APP-2f-Calculator_PF:
	-$(RM) ./APP/Calculator_PF/app_calculator_PF.cyclo ./APP/Calculator_PF/app_calculator_PF.d ./APP/Calculator_PF/app_calculator_PF.o ./APP/Calculator_PF/app_calculator_PF.su

.PHONY: clean-APP-2f-Calculator_PF

