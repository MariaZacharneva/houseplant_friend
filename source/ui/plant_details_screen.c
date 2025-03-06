/*
 * plant_details_screen.c
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#include <ui/plant_details_screen.h>
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
#include "main_screen.h"
#include "src/common.h"
#include <stdio.h>

Screen_t PlantDetailsScreen = {PlantDetailsScreenInit, PlantDetailsScreenProcess, PlantDetailsScreenDeinit, &plant_detail_screen};

int details_screen_k = 3;

lv_obj_t  *plant_detail_screen;
lv_obj_t *rename_input;
lv_obj_t *rename_btn;

void PlantDetailsScreenInit() {
	plant_detail_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(plant_detail_screen, 5, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(2), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(plant_detail_screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	// Set a border and padding for visualization
	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	// Create the common info section (row 0, span all columns)
	lv_obj_t *name_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(name_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(name_area, lv_color_make(0xEE, 0xEE, 0xEE), 0); // Background color
	lv_obj_set_style_pad_all(name_area, 10, 0);

	rename_input = lv_textarea_create(name_area);
    lv_textarea_set_text(rename_input, plant_names[selected_plant]);
	lv_obj_align(rename_input, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_size(rename_input, lv_pct(50), lv_pct(100));

    rename_btn = lv_btn_create(name_area);
	lv_obj_align(rename_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_set_size(rename_btn, lv_pct(20), lv_pct(100));
	lv_obj_t *rename_btn_label = lv_label_create(rename_btn);
	lv_label_set_text(rename_btn_label, "Rename");


	// Create the common info section (row 0, span all columns)
	lv_obj_t *info_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(info_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

	lv_obj_t *moisture_label1 = lv_label_create(info_area);
	char moisture_info1[64];
	snprintf(moisture_info1, sizeof(moisture_info1), "Moisture sensor 1: %d%%",
			GetMoisture(selected_plant, 0));
	lv_label_set_text(moisture_label1, moisture_info1);
	lv_obj_align(moisture_label1, LV_ALIGN_TOP_LEFT, 0, 0);

	lv_obj_t *moisture_label2 = lv_label_create(info_area);
	char moisture_info2[64];
	snprintf(moisture_info2, sizeof(moisture_info2), "Moisture sensor 2: %d%%",
			GetMoisture(selected_plant, 1));
	lv_label_set_text(moisture_label2, moisture_info2);
	lv_obj_align(moisture_label2, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t *watering_label = lv_label_create(info_area);
	char watering_info[64];
	snprintf(watering_info, sizeof(watering_info), "Last watering: 12:00");
	lv_label_set_text(watering_label, watering_info);
	lv_obj_align(watering_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);

}

void DetailsScreen_UpdateSelectionHighlight() {
	lv_obj_set_style_bg_color(rename_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(rename_btn, lv_color_make(0x66, 0x99, 0xCC), 0);
	if (focus == 1) {
		lv_obj_set_style_bg_color(rename_input, lv_color_make(0xb3, 0xce, 0xe5), 0);
	} else if (focus == 2) {
		lv_obj_set_style_bg_color(rename_btn, lv_color_make(0x00, 0x88, 0x88), 0);
	}
}

void DetailsScreen_ProcessKeyboardInput(Key_t key) {
	if (text_input_mode) {
		EnterText(key, rename_input);
		return;
	}
    switch (KeyFunc(key.key)) {
        case KEY_UP:
        	focus = (focus - 1 + details_screen_k) % details_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % details_screen_k;
            break;
        case KEY_SELECT:
        	if (focus == 1) {
        		StartTextInput(rename_input);
        	} else if (focus == 2) {
        		strcpy(plant_names[selected_plant], lv_textarea_get_text(rename_input));
        	    focus = 0;
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void PlantDetailsScreenProcess(void) {
	DetailsScreen_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		DetailsScreen_ProcessKeyboardInput(key);
	}
}
void PlantDetailsScreenDeinit(void) {}
