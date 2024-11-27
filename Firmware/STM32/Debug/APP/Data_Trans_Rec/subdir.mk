################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/Data_Trans_Rec/app_data_trans_rec.c 

OBJS += \
./APP/Data_Trans_Rec/app_data_trans_rec.o 

C_DEPS += \
./APP/Data_Trans_Rec/app_data_trans_rec.d 


# Each subdirectory must supply rules for building sources it contributes
APP/Data_Trans_Rec/%.o APP/Data_Trans_Rec/%.su APP/Data_Trans_Rec/%.cyclo: ../APP/Data_Trans_Rec/%.c APP/Data_Trans_Rec/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DSTM32F103xB -DUSE_FULL_LL_DRIVER -DHSE_VALUE=8000000 -DHSE_STARTUP_TIMEOUT=100 -DLSE_STARTUP_TIMEOUT=5000 -DLSE_VALUE=32768 -DHSI_VALUE=8000000 -DLSI_VALUE=40000 -DVDD_VALUE=3300 -DPREFETCH_ENABLE=1 -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../BSP/ADC -I../BSP/UART -I../BSP/GPIO -I../Middleware/Ring_Buffer -I../BSP/DMA -I../BSP/TIMER -I../APP/Common -I../Scheduler -I../APP/Calculator_PF -I../APP/Read_Peek_Voltage_Current -I../BSP/SYSTICK -I../APP/Data -I../APP/CMDLine/Include -I../Device/ACS712 -I../APP/Led_7_Seg -I../APP/Data_Trans_Rec -I../APP/Status_Led -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-APP-2f-Data_Trans_Rec

clean-APP-2f-Data_Trans_Rec:
	-$(RM) ./APP/Data_Trans_Rec/app_data_trans_rec.cyclo ./APP/Data_Trans_Rec/app_data_trans_rec.d ./APP/Data_Trans_Rec/app_data_trans_rec.o ./APP/Data_Trans_Rec/app_data_trans_rec.su

.PHONY: clean-APP-2f-Data_Trans_Rec

