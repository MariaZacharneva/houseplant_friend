/*
 * pump_tasks.h
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#ifndef PUMP_TASKS_H_
#define PUMP_TASKS_H_

#define NUM_PUMPS 4

void PumpControllerTask(void *param);

void ControlPump(uint8_t pump_num, uint8_t turn_on);

#endif /* PUMP_TASKS_H_ */
