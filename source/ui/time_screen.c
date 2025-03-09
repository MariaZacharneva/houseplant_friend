/*
 * time_screen.c
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#include <ui/time_screen.h>

/*
 * time_screen.c
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#include "time_screen.h"

#include "src/common.h"
#include <stdio.h>
#include <stdlib.h>
#include "lvgl.h"
#include "tasks/ext_clock_tasks.h"

#define TIME_INPUTS 5

Screen_t TimeScreen = {TimeScreenInit, TimeScreenProcess, TimeScreenDeinit, &time_screen};

lv_obj_t *time_screen;

int time_screen_k = 7;

lv_obj_t *time_inputs[TIME_INPUTS] = {0};

lv_obj_t *save_btn;

// UI time
// ------------------------------------------------------------------------------
void TimeScreenInit(void) {
	time_screen = lv_obj_create(NULL);
	lv_obj_set_style_pad_all(time_screen, 5, 0); // Add padding around the screen

	// Create a parent container with a grid layout
	static lv_coord_t grid_template_rows[] = {LV_GRID_FR(1), LV_GRID_FR(3), LV_GRID_TEMPLATE_LAST};
	static lv_coord_t grid_template_cols[] = {LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
	lv_obj_t *main_container = lv_obj_create(time_screen);
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

    lv_obj_t *time_label = lv_label_create(header_area);
    lv_label_set_text(time_label, "Time Settings");
    lv_obj_align(time_label, LV_ALIGN_LEFT_MID, 0, 0);

	lv_obj_t *main_area = lv_obj_create(main_container);
	lv_obj_set_grid_cell(main_area, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_STRETCH, 1, 1);
	lv_obj_set_style_border_color(main_area, lv_color_make(0xEE, 0xEE, 0xEE), 0);
	lv_obj_set_style_pad_all(main_area, 10, 0);

	static lv_style_t style;
	lv_style_init(&style);
	lv_style_set_text_font(&style, &lv_font_montserrat_24); // Set font size

	int offset = 30;

	lv_obj_t *label1 = lv_label_create(main_area);
	lv_label_set_text(label1, ":");
	lv_obj_align(label1, LV_ALIGN_LEFT_MID, 90, 0);
	lv_obj_add_style(label1, &style, LV_PART_MAIN);

	lv_obj_t *label2 = lv_label_create(main_area);
	lv_label_set_text(label2, "-");
	lv_obj_align(label2, LV_ALIGN_LEFT_MID, 260, 0);
	lv_obj_add_style(label2, &style, LV_PART_MAIN);

	lv_obj_t *label3 = lv_label_create(main_area);
	lv_label_set_text(label3, "-");
	lv_obj_align(label3, LV_ALIGN_LEFT_MID, 330, 0);
	lv_obj_add_style(label3, &style, LV_PART_MAIN);

	int placeholders[TIME_INPUTS] = {global_daytime.hour, global_daytime.min,
			global_daytime.day, global_daytime.month, global_daytime.year};
	char buff[10];
	for (int i = 0; i < TIME_INPUTS; i++) {
		time_inputs[i] = lv_textarea_create(main_area);
		lv_obj_add_style(time_inputs[i], &style, LV_PART_MAIN);
		lv_obj_set_size(time_inputs[i], i == 4 ? lv_pct(20) : lv_pct(13), lv_pct(40));
		itoa(placeholders[i], buff, 10);
	    lv_textarea_set_text(time_inputs[i], buff);
	    lv_obj_align(time_inputs[i], LV_ALIGN_LEFT_MID, offset, 0);
	    offset += (i == 1 ? 100 : 70);
	}

    save_btn = lv_btn_create(main_area);
	lv_obj_align(save_btn, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
	lv_obj_t *save_label = lv_label_create(save_btn);
	lv_obj_set_style_bg_color(save_btn, Blue(), 0);
	lv_label_set_text(save_label, "Save");
}

void TimeScreen_UpdateSelectionHighlight() {
	for (int i = 0; i < TIME_INPUTS; i++) {
		lv_obj_set_style_bg_color(time_inputs[i], lv_color_white(), 0);
	}
	lv_obj_set_style_bg_color(save_btn, Blue(), 0);

	if (focus >= 1 && focus <= 5) {
		lv_obj_set_style_bg_color(time_inputs[focus - 1], lv_color_make(0xc0, 0xc0, 0xc0), 0);
	} else if (focus == 6) {
		lv_obj_set_style_bg_color(save_btn, DeepBlue(), 0);
	}
}

void UpdateDayTime() {
	DayTime_t updated_daytime;
	updated_daytime.hour = atoi(lv_textarea_get_text(time_inputs[0])) % 60;
	updated_daytime.min = atoi(lv_textarea_get_text(time_inputs[1])) % 60;
	updated_daytime.day = atoi(lv_textarea_get_text(time_inputs[2])) % 31;
	updated_daytime.month = atoi(lv_textarea_get_text(time_inputs[3])) % 12;
	updated_daytime.year = atoi(lv_textarea_get_text(time_inputs[4]));
    xQueueSend(xQueueSetDayTime, (void*)&updated_daytime, (TickType_t )0);
}

void TimeScreen_ProcessKeyboardInput(Key_t key) {
	if (text_input_mode) {
		EnterText(key, time_inputs[focus - 1]);
		return;
	}
    switch (KeyFunc(key.key)) {
        case KEY_UP:
        	focus = (focus - 1 + time_screen_k) % time_screen_k;
            break;
        case KEY_DOWN:
        	focus = (focus + 1) % time_screen_k;
            break;
        case KEY_SELECT:
        	if (focus >= 1 && focus <= 5) {
        		StartTextInput(time_inputs[focus - 1], 1);
        	} else if (focus == 6) {
        		UpdateDayTime();
        		focus = 0;
        	}
            break;
        case KEY_BACK:
        	LoadScreen(&MainScreen);
            break;
    }
}

void TimeScreenProcess(void) {
	TimeScreen_UpdateSelectionHighlight();

	Key_t key = ListenToKeyboard();
	if (key.key != '\0') {
		TimeScreen_ProcessKeyboardInput(key);
	}
}

void TimeScreenDeinit() {}
