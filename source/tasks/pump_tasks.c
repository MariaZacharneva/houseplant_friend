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
	int low_th = GetLowThreshold(plant);
	int high_th = GetHighThreshold(plant);
	if (low_th < 0 || high_th < 0) {
		return false;
	}
	int sensor1 = GetMoisture(plant, 0);
	int sensor2 = GetMoisture(plant, 0);
	if (WATERING & (1 << plant)) {
		return sensor1 < high_th && sensor2 < high_th;
	}
	return sensor1 < low_th && sensor2 < low_th;
}

void ControlPump(uint8_t pump_num, uint8_t turn_on) {
	MCP23017_SetPin(MCP23017_1_ADDRESS, GPA, pump_num, 1 - turn_on);
}

void PumpControllerTask(void *param) {
	for (uint8_t i = 0; i < NUM_PUMPS; i++) {
		ControlPump(i, 0);
	}

	while(1) {
		int watering = 0;

		for (uint8_t i = 0; i < NUM_PLANTS; i++) {
			if (!PlantData[i].active) {
				continue;
			}
			if (CheckEnableWatering(i)) {
				ControlPump(PlantData[i].pumpId, 1);
				SetWateringTime(i, xTaskGetTickCount());
				watering = watering | (1 << i);
			} else {
				ControlPump(PlantData[i].pumpId, 0);
				watering = watering & ~(1 << i);
			}
		}
		WATERING = watering;
		if (WATERING == 0) {
			vTaskDelay(pdMS_TO_TICKS(2000));
		} else {
			// Faster cycles when watering is in process.
			vTaskDelay(pdMS_TO_TICKS(100));
		}
	}
}
