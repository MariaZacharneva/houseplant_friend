/*
 * manager.c
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#include "FreeRTOS.h"
#include "fsl_debug_console.h"
#include <string.h>

#include "ui/manager.h"
#include "src/common.h"

int focus = 0;
Screen_t* curr_screen;
int selected_plant = 0;
int text_input_mode = 0;

char plant_names[4][32] = {"Plant 0", "Plant 1", "Plant 2", "Plant 3"};

lv_color_t Green() {
    return lv_color_make(34, 177, 76);
}

lv_color_t Red() {
    return lv_color_make(200, 40, 40);
}

lv_color_t Yellow() {
    return lv_color_make(255, 198, 0);
}

Key_t ListenToKeyboard() {
	Key_t key;
	key.key = '\0';
	if (xQueueReceive(xQueueKeyboard, &key, ( TickType_t ) 10)) {
		return key;
	}
	return key;
}

void LoadScreen(Screen_t *screen) {
	screen->init();
	focus = 0;
	text_input_mode = 0;
	lv_scr_load_anim(*(screen->screen_obj), LV_SCR_LOAD_ANIM_NONE, 0, 0, true);
	curr_screen = screen;
}

char KeyFunc(char key) {
	if (strchr(key_map[0][2], key) != NULL) {
		return KEY_UP;
	}
	if (strchr(key_map[1][3], key) != NULL) {
		return KEY_LEFT;
	}
	if (strchr(key_map[1][1], key) != NULL) {
		return KEY_RIGHT;
	}
	if (strchr(key_map[2][2], key) != NULL) {
		return KEY_DOWN;
	}
	if (strchr(key_map[3][0], key) != NULL) {
		return KEY_SELECT;
	}
	if (strchr(key_map[0][0], key) != NULL) {
		return KEY_BACK;
	}
	if (strchr(key_map[1][0], key) != NULL) {
		return KEY_DEL;
	}
}

void StartTextInput(lv_obj_t* text_input) {
	text_input_mode = 1;
    lv_obj_add_state(text_input, LV_STATE_FOCUSED);
}

void StopTextInput(lv_obj_t* text_input) {
	text_input_mode = 0;
	lv_obj_clear_state(text_input, LV_STATE_FOCUSED);
}

void EnterText(Key_t key, lv_obj_t* text_input) {
	if (KeyFunc(key.key) == KEY_SELECT) {
		StopTextInput(text_input);
		return;
	}
	if (KeyFunc(key.key) == KEY_DEL) {
    	lv_textarea_del_char(text_input);
		return;
	}
    char txt[2] = {key.key, '\0'};
    if (key.override) {
    	lv_textarea_del_char(text_input);
    }
	lv_textarea_set_cursor_pos(text_input, LV_TEXTAREA_CURSOR_LAST);
	lv_textarea_add_char(text_input, key.key);
}
