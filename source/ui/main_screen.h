/*
 * main_screen.h
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#ifndef UI_MAIN_SCREEN_H_
#define UI_MAIN_SCREEN_H_

#include "manager.h"
#include "lvgl.h"

extern lv_obj_t *main_screen;

extern lv_obj_t* temperature_label;
extern lv_obj_t* humidity_label;

extern lv_obj_t* wifi_btn;
extern lv_obj_t* plant_btn[4];


void MainScreenInit(void);
void MainScreenProcess(void);
void MainScreenDeinit(void);

#endif /* UI_MAIN_SCREEN_H_ */
