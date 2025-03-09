/*
 * time_screen.h
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#ifndef UI_TIME_SCREEN_H_
#define UI_TIME_SCREEN_H_

#include "lvgl.h"
#include "manager.h"

extern lv_obj_t *time_screen;

void TimeScreenInit(void);
void TimeScreenProcess(void);
void TimeScreenDeinit(void);

#endif /* UI_TIME_SCREEN_H_ */
