/*
 * common.h
 *
 *  Created on: Jan 24, 2025
 *      Author: Masha
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stddef.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"

#define NUM_PLANTS 4
#define DHT11_SENSORS 2
#define INVALIDE_VAL -1

extern void PrintStackInfo(char* str);

typedef struct _plant_data {
	uint8_t enabled;
	uint8_t moistSensorIds[2];
	uint8_t pumpId;

	int moistSensorValues[2];
	SemaphoreHandle_t xMoistSensorMutex;

	int moistSensorThr[2];
	SemaphoreHandle_t xMoistThrMutex;
} plant_data_t;

extern plant_data_t PlantData[NUM_PLANTS];

extern int DHT11SensorValues[DHT11_SENSORS];
extern SemaphoreHandle_t xDHT11SensorMutex;

extern int DHT11SensorThresholds[DHT11_SENSORS];
extern SemaphoreHandle_t xDHT11ThrMutex;

extern uint8_t IS_WATERING;

void Plants_Init();

int GetMoisture(uint8_t plant_id, uint8_t sensor_num);
uint8_t SetMoisture(uint8_t plant_id, uint8_t sensor_num, int val);

int GetMoistureThreshold(uint8_t plant_id, uint8_t sensor_num);
uint8_t SetMoistureThr(uint8_t plant_id, uint8_t sensor_num, int val);

int GetTemperature();
uint8_t SetTemperature(int temp);

int GetHumidity();
uint8_t SetHumidity(int hum);

typedef struct {
	char key;
	bool override;
} Key_t;

extern QueueHandle_t xQueueKeyboard;

extern char key_map[4][4][5];


#endif /* TASKS_COMMON_H_ */
