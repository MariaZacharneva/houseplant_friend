/*
 * lvgl_tasks.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#ifndef LVGL_TASKS_H_
#define LVGL_TASKS_H_

extern volatile bool s_lvgl_initialized;

#define LED_GPIO       GPIO1
#define LED_GPIO_PIN   24      // pin D3 (J22/4)

void DisplayImage(void);

void DisplayTask(void *param);

void KeyboardTask(void *param);

void LEDTask(void *param);

#endif /* LVGL_TASKS_H_ */
