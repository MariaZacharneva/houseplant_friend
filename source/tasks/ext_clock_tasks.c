/*
 * ext_clock_tasks.c
 *
 *  Created on: Mar 9, 2025
 *      Author: Masha
 */

#include <tasks/ext_clock_tasks.h>

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"

#include <stdio.h>
#include "src/lpi2c_func.h"
#include "src/common.h"

#define DS1307_I2C_ADDR  0x68

QueueHandle_t xQueueSetDayTime;
DayTime_t global_daytime;

void DS1307_Write(uint8_t reg, uint8_t value) {
    uint8_t data[2] = { reg, value };
    status_t status = LPI2C_WriteData(DS1307_I2C_ADDR, data, sizeof(data));
    if (status != kStatus_Success) {
    	PRINTF("---> DS1307_Write Error\r\n");
    }
}

uint8_t DS1307_Read(uint8_t reg) {
	uint8_t value;
    status_t status = LPI2C_ReadData(DS1307_I2C_ADDR, reg, &value);
    if (status != kStatus_Success) {
    	PRINTF("---> DS1307_Read Error\r\n");
    }
    return value;
}

uint8_t decToBcd(uint8_t val) {
    return ((val / 10) << 4) | (val % 10);
}

uint8_t bcdToDec(uint8_t val) {
    return ((val >> 4) * 10) + (val & 0x0F);
}

void DS1307_SetTime(DayTime_t daytime) {
    DS1307_Write(0x00, decToBcd(daytime.sec) & 0x7F); // Seconds (clearing CH bit)
    DS1307_Write(0x01, decToBcd(daytime.min)); // Minutes
    DS1307_Write(0x02, decToBcd(daytime.hour)); // Hours (24-hour format)
    DS1307_Write(0x04, decToBcd(daytime.day));  // Day of month
    DS1307_Write(0x05, decToBcd(daytime.month));  // Month
    uint8_t year = daytime.year - 2000;
    DS1307_Write(0x06, decToBcd(year)); // Year (2025 - 2000)
    printf("RTC time set successfully!\n");
}

void DS1307_GetTime() {
	global_daytime.sec = bcdToDec(DS1307_Read(0x00) & 0x7F);
	global_daytime.min = bcdToDec(DS1307_Read(0x01));
	global_daytime.hour = bcdToDec(DS1307_Read(0x02));
	global_daytime.day = bcdToDec(DS1307_Read(0x04));
	global_daytime.month = bcdToDec(DS1307_Read(0x05));
	global_daytime.year = bcdToDec(DS1307_Read(0x06)) + 2000;
}

void DS1307_Task(void *param)
{
	xQueueSetDayTime = xQueueCreate(8, sizeof(DayTime_t));

	DayTime_t daytime;
    while (1)
    {
		if (xQueueReceive(xQueueSetDayTime, &daytime, ( TickType_t ) 0)) {
		    DS1307_SetTime(daytime);
		}
        DS1307_GetTime();
        vTaskDelay(pdMS_TO_TICKS(10 * 1000)); // 10 sec delay
    }
}
