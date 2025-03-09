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

int details_screen_k = 7;

lv_obj_t *plant_detail_screen;
lv_obj_t *rename_input;
lv_obj_t *rename_btn;
lv_obj_t *activate_btn;
lv_obj_t *info_label;
lv_obj_t *smily;
lv_obj_t *low_th_input;
lv_obj_t *high_th_input;
lv_obj_t *watering_btn;
lv_obj_t *watering_label;


void PlantDetailsScreenInit() {
	plant_detail_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(plant_detail_screen, 5, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(plant_detail_screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	lv_obj_t *name_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(name_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(name_area, lv_color_make(0xEE, 0xEE, 0xEE), 0); // Background color
	lv_obj_set_style_pad_all(name_area, 5, 0);

	rename_input = lv_textarea_create(name_area);
    lv_textarea_set_text(rename_input, plant_names[selected_plant]);
	lv_obj_align(rename_input, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_size(rename_input, lv_pct(40), lv_pct(100));

    rename_btn = lv_btn_create(name_area);
	lv_obj_align(rename_btn, LV_ALIGN_CENTER, 0, 0);
//	lv_obj_set_size(rename_btn, lv_pct(20), lv_pct(90));
	lv_obj_t *rename_btn_label = lv_label_create(rename_btn);
	lv_obj_set_style_bg_color(rename_btn, Blue(), 0);
	lv_label_set_text(rename_btn_label, "Rename");

	activate_btn = lv_btn_create(name_area);
	lv_obj_align(activate_btn, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_t *activate_btn_label = lv_label_create(activate_btn);
	lv_obj_set_style_bg_color(activate_btn, Blue(), 0);
	lv_label_set_text(activate_btn_label, "Deactiv.");

	static lv_coord_t rows[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t cols[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *plant_grid = lv_obj_create(main_container);
	lv_obj_set_grid_cell(plant_grid, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_grid_dsc_array(plant_grid, cols, rows);
	lv_obj_set_style_border_color(plant_grid, lv_color_white(), 0);
	lv_obj_set_style_pad_all(plant_grid, 0, 0);

	lv_obj_t *info_area = lv_obj_create(plant_grid);
	lv_obj_set_grid_cell(info_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

	info_label = lv_label_create(info_area);
	lv_label_set_text(info_label, "Loading...");
	lv_obj_align(info_label, LV_ALIGN_TOP_LEFT, 0, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_text_font(&style, &lv_font_montserrat_24); // Set font size

	smily = lv_label_create(info_area);
	lv_obj_add_style(smily, &style, LV_PART_MAIN);
	lv_label_set_text(smily, "(^_^)");
	lv_obj_align(smily, LV_ALIGN_BOTTOM_LEFT, 0, 0);

	lv_obj_t *action_area = lv_obj_create(plant_grid);
	lv_obj_set_grid_cell(action_area, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_STRETCH, 0, 1);

	lv_obj_t *low_th_label = lv_label_create(action_area);
	lv_label_set_text(low_th_label, "Low threshold:");
	lv_obj_align(low_th_label, LV_ALIGN_TOP_LEFT, 0, 10);

	low_th_input = lv_textarea_create(action_area);
	lv_obj_align(low_th_input, LV_ALIGN_TOP_RIGHT, 0, 0);
	lv_obj_set_size(low_th_input, lv_pct(50), lv_pct(25));
	char buf[32];
    snprintf(buf, sizeof(buf), "%d", GetLowThreshold(selected_plant));
    lv_textarea_set_text(low_th_input, buf);

	lv_obj_t *high_th_label = lv_label_create(action_area);
	lv_label_set_text(high_th_label, "High threshold:");
	lv_obj_align(high_th_label, LV_ALIGN_LEFT_MID, 0, 0);

	high_th_input = lv_textarea_create(action_area);
	lv_obj_align(high_th_input, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_set_size(high_th_input, lv_pct(50), lv_pct(25));
    snprintf(buf, sizeof(buf), "%d", GetHighThreshold(selected_plant));
    lv_textarea_set_text(high_th_input, buf);

	watering_btn = lv_btn_create(action_area);
	lv_obj_align(watering_btn, LV_ALIGN_BOTTOM_LEFT, 0, 0);
	watering_label = lv_label_create(watering_btn);
	lv_obj_set_style_bg_color(watering_btn, Blue(), 0);
	lv_label_set_text(watering_label, "Start manual watering");
}

void ResetHighlight() {
	lv_obj_set_style_bg_color(rename_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(rename_btn, Blue(), 0);
	lv_obj_set_style_bg_color(low_th_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(high_th_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(watering_btn, Blue(), 0);
	lv_obj_set_style_bg_color(activate_btn, Blue(), 0);
}

void DetailsScreen_UpdateSelectionHighlight() {
	ResetHighlight();
	switch(focus) {
	case 1:
		lv_obj_set_style_bg_color(rename_input, LightBlue(), 0);
		break;
	case 2:
		lv_obj_set_style_bg_color(rename_btn, DeepBlue(), 0);
		break;
	case 3:
		lv_obj_set_style_bg_color(low_th_input, LightBlue(), 0);
		break;
	case 4:
		lv_obj_set_style_bg_color(high_th_input, LightBlue(), 0);
		break;
	case 5:
		lv_obj_set_style_bg_color(watering_btn, DeepBlue(), 0);
		break;
	case 6:
		lv_obj_set_style_bg_color(activate_btn, DeepBlue(), 0);
		break;
	default:
		break;
	}
}

void UpdatePlantData() {
	char info[128];
	char time_str[32];
	int watering_time = GetWateringTime(selected_plant);
	if (watering_time == -1) {
        snprintf(time_str, sizeof(time_str), "never");
	} else if (WATERING & (1 << selected_plant)) {
        snprintf(time_str, sizeof(time_str), "NOW");
	} else {
		TickToRealTime(time_str, xTaskGetTickCount() - watering_time);
	}

	snprintf(info, sizeof(info), "Moisture sensor 1: %d%%\nMoisture sensor 2: %d%%\nLast watering: %s",
			GetMoisture(selected_plant, 0),
			GetMoisture(selected_plant, 1),
			time_str);
	lv_label_set_text(info_label, info);

	if (!text_input_mode) {
		int low_th = atoi(lv_textarea_get_text(low_th_input));
		if (GetLowThreshold(selected_plant) != low_th) {
			SetLowThreshold(selected_plant, low_th);
		}

		int high_th = atoi(lv_textarea_get_text(high_th_input));
		if (GetHighThreshold(selected_plant) != high_th) {
			SetHighThreshold(selected_plant, high_th);
		}
	}
}

void DetailsScreen_ProcessKeyboardInput(Key_t key) {
	if (text_input_mode) {
		switch (focus) {
		case 1:
			EnterText(key, rename_input);
			break;
    	case 3:
    		EnterText(key, low_th_input);
    		break;
    	case 4:
    		EnterText(key, high_th_input);
    		break;
		}
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
        	switch(focus) {
        	case 1:
        		StartTextInput(rename_input, 0);
        		break;
        	case 2:
        		strcpy(plant_names[selected_plant], lv_textarea_get_text(rename_input));
        	    focus = 0;
        	    break;
        	case 3:
        		StartTextInput(low_th_input, 1);
        		break;
        	case 4:
        		StartTextInput(high_th_input, 1);
        		break;
        	case 5:
        		// start or stop watering...
        		break;
        	case 6:
        		SetActive(selected_plant, false);
            	LoadScreen(&InactivePlantScreen);
        		break;
        	default:
        		break;
        	}
    		break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void PlantDetailsScreenProcess(void) {
	DetailsScreen_UpdateSelectionHighlight();
	UpdatePlantData();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		DetailsScreen_ProcessKeyboardInput(key);
	}
}
void PlantDetailsScreenDeinit(void) {}
