/*
 * lvgl_tasks.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "lvgl_support.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "lvgl.h"
#include "lvgl_tasks.h"

#include "src/common.h"
#include "ui/main_screen.h"
#include "ui/wifi_screen.h"
#include "ui/plant_details_screen.h"

#include <stdio.h>

volatile bool s_lvgl_initialized = false;

void DisplayTask(void *param) {
#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif
    PRINTF("lvgl widgets demo started\r\n");
    lv_port_pre_init();
    lv_init();
    lv_port_disp_init();

    s_lvgl_initialized = true;

	LoadScreen(&MainScreen);

    while (true)
    {
    	curr_screen->process();
		lv_task_handler();
		vTaskDelay(pdMS_TO_TICKS(200));
    }
}
