/*
 * sensor_tasks.c
 *
 *  Created on: Jan 23, 2025
 *      Author: Masha
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_adc.h"
#include "fsl_debug_console.h"
#include "semphr.h"
#include "src/lpi2c_func.h"
#include "sensor_tasks.h"
#include "pump_tasks.h"
#include "src/common.h"

#define ADC_BASE ADC1
#define ADC_CHANNEL_GROUP 0U
#define ADC_USER_CHANNEL 15


void CustomADC_Init() {
    CLOCK_EnableClock(kCLOCK_Adc1);

    adc_config_t adcConfig;
    ADC_GetDefaultConfig(&adcConfig);
    ADC_Init(ADC_BASE, &adcConfig);
}

uint32_t readADCValue()
{
    adc_channel_config_t channelConfig;
    channelConfig.channelNumber = ADC_USER_CHANNEL;
    channelConfig.enableInterruptOnConversionCompleted = false;
    ADC_SetChannelConfig(ADC_BASE, ADC_CHANNEL_GROUP, &channelConfig);

    uint32_t timeout = 10000000;
    while (!ADC_GetChannelStatusFlags(ADC_BASE, ADC_CHANNEL_GROUP) && --timeout) {}

    if (timeout == 0)
    {
    	PRINTF("ADC conversion timed out \r\n");
    }
    return ADC_GetChannelConversionValue(ADC_BASE, ADC_CHANNEL_GROUP);
}

void ListenSensor(uint8_t sensor_num) {
	MCP23017_SetPin(MCP23017_1_ADDRESS, GPB, 0, sensor_num & 1);
	MCP23017_SetPin(MCP23017_1_ADDRESS, GPB, 1, (sensor_num >> 1) & 1);
	MCP23017_SetPin(MCP23017_1_ADDRESS, GPB, 2, (sensor_num >> 2) & 1);
}

void UpdateSensors() {
	for (uint8_t p = 0; p < NUM_PLANTS; p++) {
		if (!PlantData[p].enabled) {
			continue;
		}
		for (uint8_t i = 0; i < 2; i++) {
			uint8_t sensor_id = PlantData[p].moistSensorIds[i];
			ListenSensor(sensor_id);
			uint32_t sensorValue = 100 - readADCValue() * 100 / 4096;
			SetMoisture(p, i, sensorValue);
		}
	}
}


void UpdateSensorsTask(void *param) {
    while (1)
    {
    	UpdateSensors();

    	// Updating sensors faster when the watering is in process.
    	vTaskDelay(pdMS_TO_TICKS(IS_WATERING ? 100 : 1000));
    }
}
