/*
 * dhg11_tasks.c
 *
 *  Created on: Jan 25, 2025
 *      Author: Masha
 */

#ifndef TASKS_DHG11_TASKS_C_
#define TASKS_DHG11_TASKS_C_

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"

#include "src/dht11.h"
#include "src/common.h"

void DHTTask(void *param)
{
    int temperature = 0, humidity = 0;

    vTaskDelay(pdMS_TO_TICKS(1000));

    DHT11_Init();
    while (1)
    {
    	int error = DHT_ReadData(&temperature, &humidity);
        if (error == 0)
        {
	        SetTemperature(temperature);
	        SetHumidity(humidity);
        }
        else
        {
        	PRINTF("Failed to read DHT11 data: code %d \r\n", error);
        }
        vTaskDelay(pdMS_TO_TICKS(3000)); // Read every 3 seconds
    }
}


#endif /* TASKS_DHG11_TASKS_C_ */
