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
#include <stdio.h>


volatile bool s_lvgl_initialized = false;
static lv_obj_t* temperature_label;
static lv_obj_t* humidity_label;

void init_led_pin(void) {
    // Set the IOMUXC for GPIO_B0_02 as GPIO (not an alternate function)
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_08_GPIO1_IO24, 0);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_08_GPIO1_IO24,
        IOMUXC_SW_PAD_CTL_PAD_PUS(2) |  // Pull-up resistor
        IOMUXC_SW_PAD_CTL_PAD_PUE(1) |  // Pull enabled
        IOMUXC_SW_PAD_CTL_PAD_PKE(1) |  // Pull/keeper enabled
        IOMUXC_SW_PAD_CTL_PAD_DSE(6)    // High drive strength
    );

    // Configure GPIO2, Pin 2 as an output
    gpio_pin_config_t led_config = {
        kGPIO_DigitalOutput,  // Output mode
        0                     // Initially set to LOW
    };
    GPIO_PinInit(LED_GPIO, LED_GPIO_PIN, &led_config);
}

void LEDTask(void *param)
{
	init_led_pin();

    PRINTF("STARTING LED BLINKING.\r\n");
    // Set up the text
    bool led_on = true;

    while (true)
    {
        vTaskDelay( pdMS_TO_TICKS(1000) );
        GPIO_PinWrite(LED_GPIO, LED_GPIO_PIN, led_on);  // Turn LED ON
        led_on = !led_on;
    }
}

void UpdatePlantDashboard(void) {
	int temp = 8;
	int hum = 8;
    char buffer[32];
    char int_buffer[32];

    // Update temperature
    temp = GetTemperature();
    lv_label_set_text_fmt(temperature_label, "Temperature: %d C", temp);

    // Update humidity
    hum = GetHumidity();
    lv_label_set_text_fmt(humidity_label, "Humidity: %d%%", hum);

    lv_obj_invalidate(temperature_label);
    lv_obj_invalidate(humidity_label);
}

void DisplayPlant(int col, int row, lv_obj_t *plant_grid) {
	int plant_id = row * 2 + col;

	lv_obj_t *cell = lv_obj_create(plant_grid);
	lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
	lv_obj_set_style_bg_color(cell, lv_color_make(0xEE, 0xEE, 0xEE), 0);
	lv_obj_set_style_pad_all(cell, 10, 0);

	// Add labels for plant name and moisture info
	lv_obj_t *plant_label = lv_label_create(cell);
	char plant_name[32];
	snprintf(plant_name, sizeof(plant_name), "Plant %d", plant_id);
	lv_label_set_text(plant_label, plant_name);
	lv_obj_align(plant_label, LV_ALIGN_TOP_MID, 0, 0);

	// Add moisture information.
	lv_obj_t *moisture_label = lv_label_create(cell);
	char moisture_info[64];
	snprintf(moisture_info, sizeof(moisture_info), "Moisture: %d%% and %d%%",
			GetMoisture(plant_id, 0), GetMoisture(plant_id, 1));
	lv_label_set_text(moisture_label, moisture_info);
	lv_obj_align(moisture_label, LV_ALIGN_BOTTOM_MID, 0, 0);
}

void DisplayImage(void) {
	    // Create a parent container for the screen
	lv_obj_t *screen = lv_scr_act();
	lv_obj_set_style_pad_all(screen, 10, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	// Set a border and padding for visualization
	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	// Create the common info section (row 0, span all columns)
	lv_obj_t *common_info = lv_obj_create(main_container);
	lv_obj_set_grid_cell(common_info, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(common_info, lv_color_make(0x00, 0x88, 0xCC), 0); // Background color
	lv_obj_set_style_pad_all(common_info, 10, 0);

	// Add labels to the common info section
	lv_obj_t *time_label = lv_label_create(common_info);

	char time_text[32];
	snprintf(time_text, sizeof(time_text), "---");
	lv_label_set_text(time_label, time_text);
	lv_obj_align(time_label, LV_ALIGN_TOP_LEFT, 0, 0);

	temperature_label = lv_label_create(common_info);
	lv_label_set_text(temperature_label, "---");
	lv_obj_align(temperature_label, LV_ALIGN_LEFT_MID, 0, 0);

	humidity_label = lv_label_create(common_info);
	lv_label_set_text(humidity_label, "---");
	lv_obj_align(humidity_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

	lv_obj_t *device_name_label = lv_label_create(common_info);
	lv_label_set_text(device_name_label, "FleurAmi");
	lv_obj_align(device_name_label, LV_ALIGN_TOP_RIGHT, 0, 0);

	// Create the 2x2 grid section (row 1, span all columns)
	static lv_coord_t plant_grid_rows[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t plant_grid_cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *plant_grid = lv_obj_create(main_container);
	lv_obj_set_grid_cell(plant_grid, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_grid_dsc_array(plant_grid, plant_grid_cols, plant_grid_rows);
	lv_obj_set_style_pad_all(plant_grid, 5, 0);

	// Create the grid cells
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 2; col++) {
			DisplayPlant(col, row, plant_grid);
		}
	}
}




void DisplayTask(void *param)
{
#if LV_USE_LOG
    lv_log_register_print_cb(print_cb);
#endif
    PRINTF("lvgl widgets demo started\r\n");
    lv_port_pre_init();
    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    s_lvgl_initialized = true;

    DisplayImage();
    while (true)
    {
    	UpdatePlantDashboard();
		lv_task_handler();
//		PrintStackInfo("display");
		vTaskDelay(pdMS_TO_TICKS(40));
    }
}
