/*
 * inactive_plant_screen.c
 *
 *  Created on: Mar 7, 2025
 *      Author: Masha
 */

#include <ui/inactive_plant_screen.h>

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

Screen_t InactivePlantScreen = {InactivePlantScreenInit, InactivePlantScreenProcess, InactivePlantScreenDeinit, &inactive_plant_screen};

int inact_screen_k = 4;

lv_obj_t *inactive_plant_screen;
lv_obj_t *inact_rename_input;
lv_obj_t *inact_rename_btn;
lv_obj_t *inact_activate_btn;

void InactivePlantScreenInit() {
	inactive_plant_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(inactive_plant_screen, 5, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(inactive_plant_screen);
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

	inact_rename_input = lv_textarea_create(name_area);
    lv_textarea_set_text(inact_rename_input, plant_names[selected_plant]);
	lv_obj_align(inact_rename_input, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_obj_set_size(inact_rename_input, lv_pct(40), lv_pct(100));

	inact_rename_btn = lv_btn_create(name_area);
	lv_obj_align(inact_rename_btn, LV_ALIGN_CENTER, 0, 0);
	lv_obj_t *rename_btn_label = lv_label_create(inact_rename_btn);
	lv_obj_set_style_bg_color(inact_rename_btn, Blue(), 0);
	lv_label_set_text(rename_btn_label, "Rename");

	inact_activate_btn = lv_btn_create(name_area);
	lv_obj_align(inact_activate_btn, LV_ALIGN_RIGHT_MID, 0, 0);
	lv_obj_t *activate_btn_label = lv_label_create(inact_activate_btn);
	lv_label_set_text(activate_btn_label, "Activate");
	lv_obj_set_style_bg_color(inact_activate_btn, Blue(), 0);

	lv_obj_t *info_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(info_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);

	lv_obj_t *info_label = lv_label_create(info_area);
	lv_obj_align(info_label, LV_ALIGN_TOP_LEFT, 0, 0);
	lv_label_set_text(info_label, "The plant is not active yet");
}

void InactivePlant_UpdateSelectionHighlight() {
	lv_obj_set_style_bg_color(inact_rename_input, lv_color_white(), 0);
	lv_obj_set_style_bg_color(inact_rename_btn, Blue(), 0);
	if (focus == 1) {
		lv_obj_set_style_bg_color(inact_rename_input, LightBlue(), 0);
	} else if (focus == 2) {
		lv_obj_set_style_bg_color(inact_rename_btn, DeepBlue(), 0);
	} else if (focus == 3) {
		lv_obj_set_style_bg_color(inact_activate_btn, DeepBlue(), 0);
	}
}

void InactivePlant_ProcessKeyboardInput(Key_t key) {
	if (text_input_mode) {
		EnterText(key, inact_rename_input);
		return;
	}
    switch (KeyFunc(key.key)) {
        case KEY_UP:
        	focus = (focus - 1 + inact_screen_k) % inact_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % inact_screen_k;
            break;
        case KEY_SELECT:
        	if (focus == 1) {
        		StartTextInput(inact_rename_input, 0);
        	} else if (focus == 2) {
        		strcpy(plant_names[selected_plant], lv_textarea_get_text(inact_rename_input));
        	    focus = 0;
        	} else if (focus == 3) {
        		SetActive(selected_plant, true);
        		SetLowThreshold(selected_plant, -1);
        		SetHighThreshold(selected_plant, -1);
            	LoadScreen(&PlantDetailsScreen);
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void InactivePlantScreenProcess(void) {
	InactivePlant_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		InactivePlant_ProcessKeyboardInput(key);
	}
}
void InactivePlantScreenDeinit(void) {}

