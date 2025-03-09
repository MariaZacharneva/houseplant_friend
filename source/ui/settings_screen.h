/*
 * settings_screen.h
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#ifndef UI_SETTINGS_SCREEN_H_
#define UI_SETTINGS_SCREEN_H_

#include "lvgl.h"
#include "manager.h"

extern lv_obj_t *settings_screen;

void SettingsScreenInit(void);
void SettingsScreenProcess(void);
void SettingsScreenDeinit(void);

#endif /* UI_SETTINGS_SCREEN_H_ */
