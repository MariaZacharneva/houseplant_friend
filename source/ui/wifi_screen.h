/*
 * wifi_screen.h
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#ifndef UI_WIFI_SCREEN_H_
#define UI_WIFI_SCREEN_H_
#include "lvgl.h"
#include "manager.h"

extern lv_obj_t  *wifi_screen;

void WiFiScreenInit(void);
void WiFiScreenProcess(void);
void WiFiScreenDeinit(void);

#endif /* UI_WIFI_SCREEN_H_ */
