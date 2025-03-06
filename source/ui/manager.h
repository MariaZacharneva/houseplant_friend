/*
 * manager.h
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#ifndef UI_MANAGER_H_
#define UI_MANAGER_H_

#include "lvgl.h"
#include "src/common.h"

#define KEY_UP    	'A'
#define KEY_DOWN  	'T'
#define KEY_LEFT  	'G'
#define KEY_RIGHT 	'M'
#define KEY_SELECT 	'#'
#define KEY_BACK   	'+'
#define KEY_DEL	  	'-'

#define MAX_WIFI_NETWORKS 10

typedef struct {
    void (*init)(void);
    void (*process)(void);
    void (*deinit)(void);
    lv_obj_t **screen_obj;
} Screen_t;

extern Screen_t MainScreen;
extern Screen_t WifiScreen;
extern Screen_t PlantDetailsScreen;

extern Screen_t* curr_screen;

extern int focus;
extern int selected_plant;
extern int text_input_mode;
extern char plant_names[4][32];

Key_t ListenToKeyboard(void);

void LoadScreen(Screen_t *screen);

char KeyFunc(char key);

void StartTextInput(lv_obj_t* text_input);

void StopTextInput(lv_obj_t* text_input);

void EnterText(Key_t key, lv_obj_t* text_input);

lv_color_t Green();

lv_color_t Red();

lv_color_t Yellow();

#endif /* UI_MANAGER_H_ */
