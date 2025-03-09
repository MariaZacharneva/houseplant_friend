/*
 * common.c
 *
 *  Created on: Jan 24, 2025
 *      Author: Masha
 */
#include "common.h"
#include <stdio.h>
#include "fsl_debug_console.h"

plant_data_t PlantData[NUM_PLANTS];

int DHT11SensorValues[DHT11_SENSORS];
SemaphoreHandle_t xDHT11SensorMutex;

int DHT11SensorThresholds[DHT11_SENSORS];
SemaphoreHandle_t xDHT11ThrMutex;

uint8_t WATERING = 0;

QueueHandle_t xQueueKeyboard;

char key_map[4][4][5] = {
    {"+", "def3", "abc2", "1"},
    {"-", "mno6", "jkl5", "ghi4"},
    {"*", "wxyz9", "tuv8", "pqrs7"},
    {"#", "]", "0", "["}
};

char digit_map[4][4][1] = {
    {"+", "3", "2", "1"},
    {"-", "6", "5", "4"},
    {"*", "9", "8", "7"},
    {"#", "]", "0", "["}
};

void CheckSemaphores() {
	for (int i = 0; i < NUM_PLANTS; i++) {
		if(PlantData[i].xMoistSensorMutex == NULL) {
	    	PRINTF("ERROR: Semaphore is null");
		}
	}
	if(xDHT11SensorMutex == NULL) {
    	PRINTF("ERROR: Semaphore is null");
	}
	if(xDHT11ThrMutex == NULL) {
    	PRINTF("ERROR: Semaphore is null");
	}
}

void Plants_Init() {
	for (int i = 0; i < NUM_PLANTS; i++) {
		PlantData[i].active = 0;
		PlantData[i].moistSensorIds[0] = 2 * i;
		PlantData[i].moistSensorIds[0] = 2 * i + 1;
		PlantData[i].xMoistSensorMutex = xSemaphoreCreateBinary();
		PlantData[i].lowMoistSensorThr = -1;
		PlantData[i].highMoistSensorThr = -1;
		PlantData[i].xMoistThrMutex = xSemaphoreCreateBinary();

		xSemaphoreGive(PlantData[i].xMoistSensorMutex);
		xSemaphoreGive(PlantData[i].xMoistThrMutex);
		PlantData[i].watering_time = -1;
	}

	// Currently only 1 plant is active.
	PlantData[0].active = 1;

	xDHT11SensorMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(xDHT11SensorMutex);

	xDHT11ThrMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(xDHT11ThrMutex);

	xQueueKeyboard = xQueueCreate(32, sizeof(Key_t));
	 if(xQueueKeyboard == NULL) {
		 PRINTF("Cannot create keyboard queue");
		 while (1) {}
	}

	CheckSemaphores();
}

bool GetActive(uint8_t plant_id) {
	return PlantData[plant_id].active;
}

void SetActive(uint8_t plant_id, bool active) {
	PlantData[plant_id].active = active;
}

int GetWateringTime(uint8_t plant_id) {
	return PlantData[plant_id].watering_time;
}

void SetWateringTime(uint8_t plant_id, int time) {
	PlantData[plant_id].watering_time = time;
}

int GetMoisture(uint8_t plant_id, uint8_t sensor_num) {
	int val;
	if (xSemaphoreTake(PlantData[plant_id].xMoistSensorMutex, 10000) == pdTRUE)
	{
		val = PlantData[plant_id].moistSensorValues[sensor_num];
		xSemaphoreGive(PlantData[plant_id].xMoistSensorMutex);
		return val;
	}
	return INVALIDE_VAL;
}

uint8_t SetMoisture(uint8_t plant_id, uint8_t sensor_num, int val) {
	if (xSemaphoreTake(PlantData[plant_id].xMoistSensorMutex, 10000) == pdTRUE)
	{
		PlantData[plant_id].moistSensorValues[sensor_num] = val;
		xSemaphoreGive(PlantData[plant_id].xMoistSensorMutex);
		return 1;
	}
	return 0;
}

int GetLowThreshold(uint8_t plant_id) {
	int val;
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		val = PlantData[plant_id].lowMoistSensorThr;
		xSemaphoreGive(PlantData[plant_id].xMoistThrMutex);
		return val;
	}
	return INVALIDE_VAL;
}

uint8_t SetLowThreshold(uint8_t plant_id, int val) {
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		PlantData[plant_id].lowMoistSensorThr = val;
		xSemaphoreGive(PlantData[plant_id].xMoistThrMutex);
		return 1;
	}
	return 0;
}

int GetHighThreshold(uint8_t plant_id) {
	int val;
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		val = PlantData[plant_id].highMoistSensorThr;
		xSemaphoreGive(PlantData[plant_id].xMoistThrMutex);
		return val;
	}
	return INVALIDE_VAL;
}

uint8_t SetHighThreshold(uint8_t plant_id, int val) {
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		PlantData[plant_id].highMoistSensorThr = val;
		xSemaphoreGive(PlantData[plant_id].xMoistThrMutex);
		return 1;
	}
	return 0;
}

uint32_t GetDHT11(uint8_t index) {
	int val;
	if (xSemaphoreTake(xDHT11SensorMutex, 10000) == pdTRUE)
	{
		val = DHT11SensorValues[index];
		xSemaphoreGive(xDHT11SensorMutex);
		return val;
	}
	return INVALIDE_VAL;
}

uint8_t SetDHT11(uint8_t index, int val) {
	if (xSemaphoreTake(xDHT11SensorMutex, 10000) == pdTRUE)
	{
		DHT11SensorValues[index] = val;
		xSemaphoreGive(xDHT11SensorMutex);
		return 1;
	}
	return 0;
}

int GetTemperature() {
	return GetDHT11(0);
}

int GetHumidity() {
	return GetDHT11(1);
}

uint8_t SetTemperature(int temp) {
	return SetDHT11(0, temp);
}

uint8_t SetHumidity(int hum) {
	return SetDHT11(1, hum);
}

void PrintStackInfo(char* str) {
	UBaseType_t freeHeap = xPortGetMinimumEverFreeHeapSize();
	PRINTF("----- Free Heap: %d ----- \r\n", freeHeap);
	UBaseType_t waterMark = uxTaskGetStackHighWaterMark(NULL);
	PRINTF("----- Task Stack High Water Mark: %s %d -----\r\n", str, waterMark);
}

int prev = 0;
void PrintEveryNSec(char* msg, int s) {
    TickType_t curr = xTaskGetTickCount();
    if (prev == 0 || curr - prev > pdMS_TO_TICKS(s * 1000)) {
    	PRINTF(msg);
    	prev = curr;
    }
}

void TickToRealTime(char* str, TickType_t ticks) {
	memset(str, 0, sizeof str);
    uint32_t ms = pdTICKS_TO_MS(ticks);
    uint32_t seconds = ms / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    minutes = minutes % 60;
    seconds = seconds % 60;
    char temp[64];
    snprintf(temp, sizeof(temp), "%d hours %d min ago", hours, minutes, seconds);
    strncat(str, temp, 1023 - strlen(str));
}

