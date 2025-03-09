/*
 * main_screen.cpp
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#include "main_screen.h"

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
#include "manager.h"
#include "src/common.h"
#include "tasks/ext_clock_tasks.h"
#include <stdio.h>

Screen_t MainScreen = {MainScreenInit, MainScreenProcess, MainScreenDeinit, &main_screen};

int main_screen_focus = 0;
int main_screen_k = 6;

lv_obj_t  *main_screen;
lv_obj_t* temperature_label;
lv_obj_t* humidity_label;
lv_obj_t* time_label;
lv_obj_t* wifi_btn;
lv_obj_t* plant_btn[4];
lv_obj_t* moisture_label[4];

void DisplayPlant(int col, int row, lv_obj_t *plant_grid) {
	int plant_id = row * 2 + col;

	lv_obj_t *cell = lv_obj_create(plant_grid);
	lv_obj_set_grid_cell(cell, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
	lv_obj_set_style_bg_color(cell, lv_color_make(0xEE, 0xEE, 0xEE), 0);
	lv_obj_set_style_pad_all(cell, 10, 0);

	// Add labels for plant name and moisture info
	lv_obj_t *plant_label = lv_label_create(cell);
	char plant_name[32];
	snprintf(plant_name, sizeof(plant_name), plant_names[plant_id]);
	lv_label_set_text(plant_label, plant_name);
	lv_obj_align(plant_label, LV_ALIGN_TOP_LEFT, 0, 0);

	// Add moisture information.
	moisture_label[plant_id] = lv_label_create(cell);
	lv_obj_align(moisture_label[plant_id], LV_ALIGN_BOTTOM_LEFT, 0, 0);

    plant_btn[plant_id] = lv_btn_create(cell);
    lv_obj_align(plant_btn[plant_id], LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_style_bg_color(plant_btn[plant_id], Blue(), 0);
    lv_obj_t *btn_label = lv_label_create(plant_btn[plant_id]);
    lv_label_set_text(btn_label, "Info");
}

void ResetPlantsHighlight() {
	for (int i = 0; i < 4; i++) {
		if (GetActive(i)) {
			lv_obj_set_style_bg_color(plant_btn[i], Blue(), 0);
	    	char moisture_info[64];
	    	if (WATERING & (1 << i)) {
	    	     snprintf(moisture_info, sizeof(moisture_info), "WATERING");
	    	} else {
		    	snprintf(moisture_info, sizeof(moisture_info), "Moisture: %d%% and %d%%",
		    			GetMoisture(i, 0), GetMoisture(i, 1));
	    	}
	    	lv_label_set_text(moisture_label[i], moisture_info);
		} else {
			lv_obj_set_style_bg_color(plant_btn[i], Gray(), 0);
	    	lv_label_set_text(moisture_label[i], "Inactive");
		}
	}
}

void MainScreenInit(void) {
	main_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(main_screen, 5, 0);

	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(2), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(main_screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	lv_obj_t *common_info = lv_obj_create(main_container);
	lv_obj_set_grid_cell(common_info, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(common_info, lv_color_make(0xEE, 0xEE, 0xEE), 0);
	lv_obj_set_style_pad_all(common_info, 10, 0);

	time_label = lv_label_create(common_info);
	lv_label_set_text(time_label, "---");
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

	wifi_btn = lv_btn_create(common_info);
	lv_obj_align(wifi_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	lv_obj_set_style_bg_color(wifi_btn, Blue(), 0);
	lv_obj_t *wifi_label = lv_label_create(wifi_btn);
	lv_label_set_text(wifi_label, "Settings");

	static lv_coord_t plant_grid_rows[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t plant_grid_cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *plant_grid = lv_obj_create(main_container);
	lv_obj_set_grid_cell(plant_grid, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_grid_dsc_array(plant_grid, plant_grid_cols, plant_grid_rows);
	lv_obj_set_style_pad_all(plant_grid, 5, 0);

	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 2; col++) {
			DisplayPlant(col, row, plant_grid);
		}
	}
	ResetPlantsHighlight();
}

void UpdatePlantDashboard(void) {
	int temp = 8;
	int hum = 8;

    temp = GetTemperature();
    lv_label_set_text_fmt(temperature_label, "Temperature: %d C", temp);

    hum = GetHumidity();
    lv_label_set_text_fmt(humidity_label, "Humidity: %d%%", hum);

	lv_label_set_text_fmt(time_label, "%04d-%02d-%02d %02d:%02d",
			global_daytime.year, global_daytime.month, global_daytime.day, global_daytime.hour, global_daytime.min);

    lv_obj_invalidate(temperature_label);
    lv_obj_invalidate(humidity_label);
}

void MainScreen_UpdateSelectionHighlight() {
	lv_obj_set_style_bg_color(wifi_btn, Blue(), 0);
	ResetPlantsHighlight();
	int main_screen_focus = focus % 6;
	if (main_screen_focus > 1 &&  main_screen_focus < 6) {
		lv_obj_set_style_bg_color(plant_btn[main_screen_focus - 2], DeepBlue(), 0);
	} else if (main_screen_focus == 1) {
		lv_obj_set_style_bg_color(wifi_btn, DeepBlue(), 0);
	}
}

void MainScreen_ProcessKeyboardInput(char key) {
    switch (KeyFunc(key)) {
        case KEY_UP:
        	focus = (focus - 1 + main_screen_k) % main_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % main_screen_k;
            break;
        case KEY_SELECT:
        	if (focus == 1) {
            	LoadScreen(&SettingsScreen);
        	} else if (focus > 0) {
        		selected_plant = focus - 2;
        		if (GetActive(selected_plant)) {
                	LoadScreen(&PlantDetailsScreen);
        		} else {
                	LoadScreen(&InactivePlantScreen);
        		}
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void MainScreenProcess() {
	UpdatePlantDashboard();
	MainScreen_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		MainScreen_ProcessKeyboardInput(key.key);
	}
}

void MainScreenDeinit() {}
