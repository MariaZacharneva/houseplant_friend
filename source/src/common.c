/*
 * common.c
 *
 *  Created on: Jan 24, 2025
 *      Author: Masha
 */
#include "common.h"
#include "fsl_debug_console.h"

plant_data_t PlantData[NUM_PLANTS];

int DHT11SensorValues[DHT11_SENSORS];
SemaphoreHandle_t xDHT11SensorMutex;

int DHT11SensorThresholds[DHT11_SENSORS];
SemaphoreHandle_t xDHT11ThrMutex;

uint8_t IS_WATERING = 0;

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
		PlantData[i].enabled = 0;
		PlantData[i].moistSensorIds[0] = 2 * i;
		PlantData[i].moistSensorIds[0] = 2 * i + 1;
		PlantData[i].xMoistSensorMutex = xSemaphoreCreateBinary();
		PlantData[i].moistSensorThr[0] = -1;
		PlantData[i].moistSensorThr[1] = -1;
		PlantData[i].xMoistThrMutex = xSemaphoreCreateBinary();

		xSemaphoreGive(PlantData[i].xMoistSensorMutex);
		xSemaphoreGive(PlantData[i].xMoistThrMutex);
	}

	// Currently only 1 plant is enabled.
	PlantData[0].enabled = 1;

	xDHT11SensorMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(xDHT11SensorMutex);

	xDHT11ThrMutex = xSemaphoreCreateBinary();
	xSemaphoreGive(xDHT11ThrMutex);

	CheckSemaphores();
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

int GetMoistureThreshold(uint8_t plant_id, uint8_t sensor_num) {
	int val;
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		val = PlantData[plant_id].moistSensorThr[sensor_num];
		xSemaphoreGive(PlantData[plant_id].xMoistThrMutex);
		return val;
	}
	return INVALIDE_VAL;
}

uint8_t SetMoistureThreshold(uint8_t plant_id, uint8_t sensor_num, int val) {
	if (xSemaphoreTake(PlantData[plant_id].xMoistThrMutex, 10000) == pdTRUE)
	{
		PlantData[plant_id].moistSensorThr[sensor_num] = val;
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
