################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../lvgl/lvgl/src/draw/lv_draw.c \
../lvgl/lvgl/src/draw/lv_draw_arc.c \
../lvgl/lvgl/src/draw/lv_draw_img.c \
../lvgl/lvgl/src/draw/lv_draw_label.c \
../lvgl/lvgl/src/draw/lv_draw_layer.c \
../lvgl/lvgl/src/draw/lv_draw_line.c \
../lvgl/lvgl/src/draw/lv_draw_mask.c \
../lvgl/lvgl/src/draw/lv_draw_rect.c \
../lvgl/lvgl/src/draw/lv_draw_transform.c \
../lvgl/lvgl/src/draw/lv_draw_triangle.c \
../lvgl/lvgl/src/draw/lv_img_buf.c \
../lvgl/lvgl/src/draw/lv_img_cache.c \
../lvgl/lvgl/src/draw/lv_img_decoder.c 

C_DEPS += \
./lvgl/lvgl/src/draw/lv_draw.d \
./lvgl/lvgl/src/draw/lv_draw_arc.d \
./lvgl/lvgl/src/draw/lv_draw_img.d \
./lvgl/lvgl/src/draw/lv_draw_label.d \
./lvgl/lvgl/src/draw/lv_draw_layer.d \
./lvgl/lvgl/src/draw/lv_draw_line.d \
./lvgl/lvgl/src/draw/lv_draw_mask.d \
./lvgl/lvgl/src/draw/lv_draw_rect.d \
./lvgl/lvgl/src/draw/lv_draw_transform.d \
./lvgl/lvgl/src/draw/lv_draw_triangle.d \
./lvgl/lvgl/src/draw/lv_img_buf.d \
./lvgl/lvgl/src/draw/lv_img_cache.d \
./lvgl/lvgl/src/draw/lv_img_decoder.d 

OBJS += \
./lvgl/lvgl/src/draw/lv_draw.o \
./lvgl/lvgl/src/draw/lv_draw_arc.o \
./lvgl/lvgl/src/draw/lv_draw_img.o \
./lvgl/lvgl/src/draw/lv_draw_label.o \
./lvgl/lvgl/src/draw/lv_draw_layer.o \
./lvgl/lvgl/src/draw/lv_draw_line.o \
./lvgl/lvgl/src/draw/lv_draw_mask.o \
./lvgl/lvgl/src/draw/lv_draw_rect.o \
./lvgl/lvgl/src/draw/lv_draw_transform.o \
./lvgl/lvgl/src/draw/lv_draw_triangle.o \
./lvgl/lvgl/src/draw/lv_img_buf.o \
./lvgl/lvgl/src/draw/lv_img_cache.o \
./lvgl/lvgl/src/draw/lv_img_decoder.o 


# Each subdirectory must supply rules for building sources it contributes
lvgl/lvgl/src/draw/%.o: ../lvgl/lvgl/src/draw/%.c lvgl/lvgl/src/draw/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1064DVL6A -DCPU_MIMXRT1064DVL6A_cm7 -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DSKIP_SYSCLK_INIT -DDATA_SECTION_IS_CACHEABLE=1 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DFSL_SDK_DRIVER_QUICK_ACCESS_ENABLE=1 -DLV_CONF_INCLUDE_SIMPLE=1 -DMCUXPRESSO_SDK -DSDK_I2C_BASED_COMPONENT_USED=1 -DSERIAL_PORT_TYPE_UART=1 -DSDK_OS_FREE_RTOS -DCR_INTEGER_PRINTF -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -D__NEWLIB__ -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\drivers" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\touchpanel" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\video" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\demos" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\demos\widgets" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\lvgl\src" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\device" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\utilities" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\uart" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\serial_manager" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\component\lists" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\xip" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\freertos\freertos-kernel\include" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\freertos\freertos-kernel\portable\GCC\ARM_CM4F" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\CMSIS" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\drivers\freertos" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\source" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\lvgl\template" -I"C:\Users\Masha\OneDrive\university\thesis\mxp_hello_world_2_lvgl\evkmimxrt1064\lvgl_demo_widgets\board" -O0 -fno-common -g3 -gdwarf-4 -c -ffunction-sections -fdata-sections -fno-builtin -Wno-format -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__NEWLIB__ -fstack-usage -specs=nano.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-lvgl-2f-lvgl-2f-src-2f-draw

clean-lvgl-2f-lvgl-2f-src-2f-draw:
	-$(RM) ./lvgl/lvgl/src/draw/lv_draw.d ./lvgl/lvgl/src/draw/lv_draw.o ./lvgl/lvgl/src/draw/lv_draw_arc.d ./lvgl/lvgl/src/draw/lv_draw_arc.o ./lvgl/lvgl/src/draw/lv_draw_img.d ./lvgl/lvgl/src/draw/lv_draw_img.o ./lvgl/lvgl/src/draw/lv_draw_label.d ./lvgl/lvgl/src/draw/lv_draw_label.o ./lvgl/lvgl/src/draw/lv_draw_layer.d ./lvgl/lvgl/src/draw/lv_draw_layer.o ./lvgl/lvgl/src/draw/lv_draw_line.d ./lvgl/lvgl/src/draw/lv_draw_line.o ./lvgl/lvgl/src/draw/lv_draw_mask.d ./lvgl/lvgl/src/draw/lv_draw_mask.o ./lvgl/lvgl/src/draw/lv_draw_rect.d ./lvgl/lvgl/src/draw/lv_draw_rect.o ./lvgl/lvgl/src/draw/lv_draw_transform.d ./lvgl/lvgl/src/draw/lv_draw_transform.o ./lvgl/lvgl/src/draw/lv_draw_triangle.d ./lvgl/lvgl/src/draw/lv_draw_triangle.o ./lvgl/lvgl/src/draw/lv_img_buf.d ./lvgl/lvgl/src/draw/lv_img_buf.o ./lvgl/lvgl/src/draw/lv_img_cache.d ./lvgl/lvgl/src/draw/lv_img_cache.o ./lvgl/lvgl/src/draw/lv_img_decoder.d ./lvgl/lvgl/src/draw/lv_img_decoder.o

.PHONY: clean-lvgl-2f-lvgl-2f-src-2f-draw

