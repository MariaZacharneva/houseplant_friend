################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.c 

C_DEPS += \
./lvgl/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.d 

OBJS += \
./lvgl/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.o 


# Each subdirectory must supply rules for building sources it contributes
lvgl/lvgl/src/draw/stm32_dma2d/%.o: ../lvgl/lvgl/src/draw/stm32_dma2d/%.c lvgl/lvgl/src/draw/stm32_dma2d/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1064DVL6A -DCPU_MIMXRT1064DVL6A_cm7 -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DSKIP_SYSCLK_INIT -DDATA_SECTION_IS_CACHEABLE=1 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 -DLV_CONF_INCLUDE_SIMPLE=1 -DMCUXPRESSO_SDK -DSDK_I2C_BASED_COMPONENT_USED=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_OS_FREE_RTOS -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\drivers" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\touchpanel" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\video" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\demos" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\demos\widgets" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\src" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\device" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\utilities" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\uart" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\serial_manager" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\lists" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\xip" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\freertos\freertos-kernel\include" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\freertos\freertos-kernel\portable\GCC\ARM_CM4F" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\CMSIS" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\drivers\freertos" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\source" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\template" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -Wno-format -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-lvgl-2f-lvgl-2f-src-2f-draw-2f-stm32_dma2d

clean-lvgl-2f-lvgl-2f-src-2f-draw-2f-stm32_dma2d:
	-$(RM) ./lvgl/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.d ./lvgl/lvgl/src/draw/stm32_dma2d/lv_gpu_stm32_dma2d.o

.PHONY: clean-lvgl-2f-lvgl-2f-src-2f-draw-2f-stm32_dma2d

