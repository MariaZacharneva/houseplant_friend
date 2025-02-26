/*
 * sensor_tasks.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#ifndef SENSOR_TASKS_H_
#define SENSOR_TASKS_H_

#include "semphr.h"

void CustomADC_Init();

void initSensorSemaphores();

void UpdateSensorsTask(void *param);
void ListenSensor(uint8_t sensor_num);

#endif /* SENSOR_TASKS_H_ */
