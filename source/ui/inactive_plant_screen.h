/*
 * inactive_plant_screen.h
 *
 *  Created on: Mar 7, 2025
 *      Author: Masha
 */

#ifndef UI_INACTIVE_PLANT_SCREEN_H_
#define UI_INACTIVE_PLANT_SCREEN_H_

#include "lvgl.h"
#include "manager.h"

extern lv_obj_t *inactive_plant_screen;

void InactivePlantScreenInit(void);
void InactivePlantScreenProcess(void);
void InactivePlantScreenDeinit(void);

#endif /* UI_INACTIVE_PLANT_SCREEN_H_ */
