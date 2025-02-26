/*
 * pump_tasks.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#include "FreeRTOS.h"
#include "task.h"

#include "clock_config.h"
#include "fsl_debug_console.h"
#include "src/lpi2c_func.h"
#include "pump_tasks.h"
#include "sensor_tasks.h"
#include "src/common.h"


bool CheckEnableWatering(uint8_t plant) {
	for (int i = 0; i < 2; i++) {
		int moistSensorVal = GetMoisture(plant, i);
		int moistSensorThresh = GetMoistureThreshold(plant, i);
		if (moistSensorVal > moistSensorThresh) {
			return false;
		}
	}
	return true;
}

void ControlPump(uint8_t pump_num, uint8_t turn_on) {
	MCP23017_SetPin(MCP23017_1_ADDRESS, GPA, pump_num, 1 - turn_on);
}

void PumpControllerTask(void *param) {
	for (uint8_t i = 0; i < NUM_PUMPS; i++) {
		ControlPump(i, 0);
	}
	while(1) {
		uint8_t watering = 0;

		for (uint8_t i = 0; i < NUM_PLANTS; i++) {
			if (!PlantData[i].enabled) {
				continue;
			}
			if (CheckEnableWatering(i)) {
				ControlPump(PlantData[i].pumpId, 1);
				watering++;
			} else {
				ControlPump(PlantData[i].pumpId, 0);
			}
		}
		if (watering == 0) {
			IS_WATERING = false;
			vTaskDelay(pdMS_TO_TICKS(2000));
		} else {
			// Faster cycles when watering is in process.
			IS_WATERING = true;
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}
