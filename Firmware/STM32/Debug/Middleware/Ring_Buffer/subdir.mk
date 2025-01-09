################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Middleware/Ring_Buffer/ring_buffer.c 

OBJS += \
./Middleware/Ring_Buffer/ring_buffer.o 

C_DEPS += \
./Middleware/Ring_Buffer/ring_buffer.d 


# Each subdirectory must supply rules for building sources it contributes
Middleware/Ring_Buffer/%.o Middleware/Ring_Buffer/%.su Middleware/Ring_Buffer/%.cyclo: ../Middleware/Ring_Buffer/%.c Middleware/Ring_Buffer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/ADC -I../BSP/UART -I../BSP/GPIO -I../Middleware/Ring_Buffer -I../BSP/DMA -I../BSP/TIMER -I../APP/Common -I../Scheduler -I../APP/Calculator_PF -I../APP/Read_Peek_Voltage_Current -I../BSP/SYSTICK -I../APP/Data -I../APP/CMDLine/Include -I../Device/ACS712 -I../APP/Led_7_Seg -I../APP/Data_Trans_Rec -I../APP/Status_Led -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Middleware-2f-Ring_Buffer

clean-Middleware-2f-Ring_Buffer:
	-$(RM) ./Middleware/Ring_Buffer/ring_buffer.cyclo ./Middleware/Ring_Buffer/ring_buffer.d ./Middleware/Ring_Buffer/ring_buffer.o ./Middleware/Ring_Buffer/ring_buffer.su

.PHONY: clean-Middleware-2f-Ring_Buffer

