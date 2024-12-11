################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Scheduler/subdir.mk
-include Middleware/Ring_Buffer/subdir.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include BSP/UART/subdir.mk
-include BSP/SYSTICK/subdir.mk
-include BSP/DMA/subdir.mk
-include BSP/ADC/subdir.mk
-include APP/Status_Led/subdir.mk
-include APP/Read_Peek_Voltage_Current/subdir.mk
-include APP/Led_7_Seg/subdir.mk
-include APP/Data_Trans_Rec/subdir.mk
-include APP/Data/subdir.mk
-include APP/Calculator_PF/subdir.mk
-include APP/CMDLine/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := MEASURE_POWER_AC
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
MEASURE_POWER_AC.elf \

MAP_FILES += \
MEASURE_POWER_AC.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
MEASURE_POWER_AC.list \


# All Target
all: main-build

# Main-build Target
main-build: MEASURE_POWER_AC.elf secondary-outputs

# Tool invocations
MEASURE_POWER_AC.elf MEASURE_POWER_AC.map: $(OBJS) $(USER_OBJS) D:\OneDrive\Disk_2\MEASURE-POWER-AC\Firmware\STM32\STM32F103C8TX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "MEASURE_POWER_AC.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\OneDrive\Disk_2\MEASURE-POWER-AC\Firmware\STM32\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="MEASURE_POWER_AC.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

MEASURE_POWER_AC.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "MEASURE_POWER_AC.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) MEASURE_POWER_AC.elf MEASURE_POWER_AC.list MEASURE_POWER_AC.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
