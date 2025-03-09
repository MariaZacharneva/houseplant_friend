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
#include "stdbool.h"

#define NUM_PLANTS 4
#define DHT11_SENSORS 2
#define INVALIDE_VAL -1

extern void PrintStackInfo(char* str);

typedef struct _plant_data {
	uint8_t active;
	uint8_t moistSensorIds[2];
	uint8_t pumpId;

	int moistSensorValues[2];
	SemaphoreHandle_t xMoistSensorMutex;

	int lowMoistSensorThr;
	int highMoistSensorThr;
	SemaphoreHandle_t xMoistThrMutex;

	int watering_time;
} plant_data_t;

extern plant_data_t PlantData[NUM_PLANTS];

extern int DHT11SensorValues[DHT11_SENSORS];
extern SemaphoreHandle_t xDHT11SensorMutex;

extern int DHT11SensorThresholds[DHT11_SENSORS];
extern SemaphoreHandle_t xDHT11ThrMutex;

extern uint8_t WATERING;

void Plants_Init();

bool GetActive(uint8_t plant_id);
void SetActive(uint8_t plant_id, bool active);

int GetWateringTime(uint8_t plant_id);
void SetWateringTime(uint8_t plant_id, int time);

int GetMoisture(uint8_t plant_id, uint8_t sensor_num);
uint8_t SetMoisture(uint8_t plant_id, uint8_t sensor_num, int val);

int GetLowThreshold(uint8_t plant_id);
uint8_t SetLowThreshold(uint8_t plant_id, int val);

int GetHighThreshold(uint8_t plant_id);
uint8_t SetHighThreshold(uint8_t plant_id, int val);

int GetTemperature();
uint8_t SetTemperature(int temp);

int GetHumidity();
uint8_t SetHumidity(int hum);

typedef struct {
	char key;
	bool override;
} Key_t;

typedef struct {
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	int year;
} DayTime_t;

extern QueueHandle_t xQueueKeyboard;

extern char key_map[4][4][5];
extern char digit_map[4][4][1];

extern int keyboard_input_mode;

void PrintEveryNSec(char* msg, int s);

void TickToRealTime(char* str, TickType_t ticks);

#endif /* TASKS_COMMON_H_ */
