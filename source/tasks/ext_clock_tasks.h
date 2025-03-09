/*
 * ext_clock_tasks.h
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#ifndef TASKS_EXT_CLOCK_TASKS_H_
#define TASKS_EXT_CLOCK_TASKS_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "src/common.h"

extern DayTime_t global_daytime;

extern QueueHandle_t xQueueSetDayTime;

void DS1307_Task(void *param);


#endif /* TASKS_EXT_CLOCK_TASKS_H_ */
