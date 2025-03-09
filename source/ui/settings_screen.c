/*
 * settings_screen.c
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#include "settings_screen.h"

#include "src/common.h"
#include "lvgl.h"

Screen_t SettingsScreen = {SettingsScreenInit, SettingsScreenProcess, SettingsScreenDeinit, &settings_screen};

lv_obj_t *settings_screen;

int settings_screen_k = 3;

lv_obj_t *wifi_settings_btn;
lv_obj_t *time_settings_btn;

// UI settings
// ------------------------------------------------------------------------------
void SettingsScreenInit(void) {
	settings_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(settings_screen, 5, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(settings_screen);
	lv_obj_set_size(main_container, lv_pct(100), lv_pct(100));
	lv_obj_set_layout(main_container, LV_LAYOUT_GRID);
	lv_obj_set_grid_dsc_array(main_container, grid_template_cols, grid_template_rows);

	// Set a border and padding for visualization
	lv_obj_set_style_border_width(main_container, 1, 0);
	lv_obj_set_style_border_color(main_container, lv_color_black(), 0);
	lv_obj_set_style_pad_all(main_container, 5, 0);

	// Create the common info section (row 0, span all columns)
	lv_obj_t *header_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(header_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 0, 1);
	lv_obj_set_style_bg_color(header_area, lv_color_make(0xEE, 0xEE, 0xEE), 0); // Background color
	lv_obj_set_style_pad_all(header_area, 10, 0);

    lv_obj_t *settings_label = lv_label_create(header_area);
    lv_label_set_text(settings_label, "Settings");
    lv_obj_align(settings_label, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t *main_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(main_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_style_bg_color(main_area, lv_color_make(0xEE, 0xEE, 0xEE), 0);
	lv_obj_set_style_pad_all(main_area, 10, 0);

    wifi_settings_btn = lv_btn_create(main_area);
	lv_obj_align(wifi_settings_btn, LV_ALIGN_TOP_MID, 0, 0);
	lv_obj_t *wifi_settings_label = lv_label_create(wifi_settings_btn);
	lv_obj_set_style_bg_color(wifi_settings_btn, Blue(), 0);
	lv_label_set_text(wifi_settings_label, "Wifi Settings");

    time_settings_btn = lv_btn_create(main_area);
	lv_obj_align(time_settings_btn, LV_ALIGN_CENTER, 0, 0);
	lv_obj_t *time_settings_label = lv_label_create(time_settings_btn);
	lv_obj_set_style_bg_color(time_settings_btn, Blue(), 0);
	lv_label_set_text(time_settings_label, "Time Settings");
}

void SettingsScreen_UpdateSelectionHighlight() {
	lv_obj_set_style_bg_color(wifi_settings_btn, Blue(), 0);
	lv_obj_set_style_bg_color(time_settings_btn, Blue(), 0);

	if (focus == 1) {
		lv_obj_set_style_bg_color(wifi_settings_btn, DeepBlue(), 0);
	} else if (focus == 2) {
		lv_obj_set_style_bg_color(time_settings_btn, DeepBlue(), 0);
	}
}

void SettingsScreen_ProcessKeyboardInput(Key_t key) {
    switch (KeyFunc(key.key)) {
        case KEY_UP:
        	focus = (focus - 1 + settings_screen_k) % settings_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % settings_screen_k;
            break;
        case KEY_SELECT:
        	if (focus == 1) {
            	LoadScreen(&WifiScreen);
        	} else if (focus == 2) {
            	LoadScreen(&TimeScreen);
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void SettingsScreenProcess(void) {
	SettingsScreen_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		SettingsScreen_ProcessKeyboardInput(key);
	}
}

void SettingsScreenDeinit() {}
