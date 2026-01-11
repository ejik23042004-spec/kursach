################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/ili9341/ds18b20.c \
../Core/ili9341/fonts.c \
../Core/ili9341/ft6336.c \
../Core/ili9341/ili9341.c \
../Core/ili9341/ili9341_touch.c \
../Core/ili9341/ow.c 

C_DEPS += \
./Core/ili9341/ds18b20.d \
./Core/ili9341/fonts.d \
./Core/ili9341/ft6336.d \
./Core/ili9341/ili9341.d \
./Core/ili9341/ili9341_touch.d \
./Core/ili9341/ow.d 

OBJS += \
./Core/ili9341/ds18b20.o \
./Core/ili9341/fonts.o \
./Core/ili9341/ft6336.o \
./Core/ili9341/ili9341.o \
./Core/ili9341/ili9341_touch.o \
./Core/ili9341/ow.o 


# Each subdirectory must supply rules for building sources it contributes
Core/ili9341/%.o Core/ili9341/%.su Core/ili9341/%.cyclo: ../Core/ili9341/%.c Core/ili9341/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L452xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/ST/touchgfx/framework/include -I../TouchGFX/generated/fonts/include -I../TouchGFX/generated/gui_generated/include -I../TouchGFX/generated/images/include -I../TouchGFX/generated/texts/include -I../TouchGFX/generated/videos/include -I../TouchGFX/gui/include -I../TouchGFX/App -I../TouchGFX/target/generated -I../TouchGFX/target -I"C:/Instik/kursach/Core/ili9341" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-ili9341

clean-Core-2f-ili9341:
	-$(RM) ./Core/ili9341/ds18b20.cyclo ./Core/ili9341/ds18b20.d ./Core/ili9341/ds18b20.o ./Core/ili9341/ds18b20.su ./Core/ili9341/fonts.cyclo ./Core/ili9341/fonts.d ./Core/ili9341/fonts.o ./Core/ili9341/fonts.su ./Core/ili9341/ft6336.cyclo ./Core/ili9341/ft6336.d ./Core/ili9341/ft6336.o ./Core/ili9341/ft6336.su ./Core/ili9341/ili9341.cyclo ./Core/ili9341/ili9341.d ./Core/ili9341/ili9341.o ./Core/ili9341/ili9341.su ./Core/ili9341/ili9341_touch.cyclo ./Core/ili9341/ili9341_touch.d ./Core/ili9341/ili9341_touch.o ./Core/ili9341/ili9341_touch.su ./Core/ili9341/ow.cyclo ./Core/ili9341/ow.d ./Core/ili9341/ow.o ./Core/ili9341/ow.su

.PHONY: clean-Core-2f-ili9341

