/*
 * plant_details_screen.h
 *
 *  Created on: Feb 28, 2025
 *      Author: Masha
 */

#ifndef UI_PLANT_DETAILS_SCREEN_H_
#define UI_PLANT_DETAILS_SCREEN_H_

#include "lvgl.h"
#include "manager.h"

extern lv_obj_t *plant_detail_screen;

void PlantDetailsScreenInit(void);
void PlantDetailsScreenProcess(void);
void PlantDetailsScreenDeinit(void);

#endif /* UI_PLANT_DETAILS_SCREEN_H_ */
