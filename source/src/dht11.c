/*
 * dht11.c
 *
 *  Created on: Jan 25, 2025
 *      Author: Masha
 */

#ifndef SRC_DHT11_C_
#define SRC_DHT11_C_

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_gpio.h"
#include "fsl_clock.h"
#include "pin_mux.h"
#include "fsl_iomuxc.h"
#include "fsl_clock.h"
#include "fsl_debug_console.h"

#define DHT11_GPIO GPIO1
#define DHT11_PIN  19
#define DHT_TIMEOUT 10000

static void DHT11_SetPinAsOutput(void) {
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalOutput,
        1, // Default high
    };
    GPIO_PinInit(DHT11_GPIO, DHT11_PIN, &pinConfig);
}

static void DHT11_SetPinAsInput(void) {
    gpio_pin_config_t pinConfig = {
        kGPIO_DigitalInput,
        0,
    };
    GPIO_PinInit(DHT11_GPIO, DHT11_PIN, &pinConfig);
}

void DHT11_Init(void) {
    CLOCK_EnableClock(kCLOCK_Iomuxc);
    IOMUXC_SetPinMux(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19, 0U);
    IOMUXC_SetPinConfig(IOMUXC_GPIO_AD_B1_03_GPIO1_IO19,
                        IOMUXC_SW_PAD_CTL_PAD_PUS(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_PUE(1U) |
                        IOMUXC_SW_PAD_CTL_PAD_SPEED(2U) |
                        IOMUXC_SW_PAD_CTL_PAD_DSE(6U));

    DHT11_SetPinAsOutput();
    GPIO_PinWrite(DHT11_GPIO, DHT11_PIN, 1);
}

int DHT_ReadBit(void)
{
    uint32_t timeout = DHT_TIMEOUT;

    // Wait for the LOW signal (50µs)
    while (!GPIO_PinRead(DHT11_GPIO, DHT11_PIN))
    {
        if (--timeout == 0) {
        	return -1; // Timeout
        }
    }

    timeout = DHT_TIMEOUT;
    // Measure the HIGH signal duration
    uint32_t highTicks = 0;
    while (GPIO_PinRead(DHT11_GPIO, DHT11_PIN))
    {
        if (--timeout == 0) {
        	return -1; // Timeout
        }
        highTicks++;
    }

    // Determine if it’s a 0 or 1
    return (highTicks > 500) ? 1 : 0;
}

int DHT_ReadByte(void)
{
    int result = 0;
    for (int i = 0; i < 8; i++)
    {
        result <<= 1;
        int bit = DHT_ReadBit();
        if (bit == -1) {
        	return -1;
        }
        result |= bit;
    }
    return result;
}

// Return codes:
// - 0: success
// - 1: timeout when waiting for the first DHT response
// - 2: timeout for DHT high-voltage signal
// - 3: data transmission didn't started
// - 4: failed when reading a byte of data
// - 5: checksum error
int DHT_ReadData(int *temperature, int *humidity)
{
    uint8_t data[5] = {0};

    // Send start signal
    DHT11_SetPinAsOutput();
    GPIO_PinWrite(DHT11_GPIO, DHT11_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(18)); // 18ms low signal
    GPIO_PinWrite(DHT11_GPIO, DHT11_PIN, 1);
    SDK_DelayAtLeastUs(18, CLOCK_GetFreq(kCLOCK_CoreSysClk)); // 18us high signal
    DHT11_SetPinAsInput();

    // Wait for DHT11 response
    uint32_t timeout = DHT_TIMEOUT;
	while (GPIO_PinRead(DHT11_GPIO, DHT11_PIN) == 1) {
        if (--timeout == 0) {
        	return 1; // Timeout
        }
        SDK_DelayAtLeastUs(1, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    }
	//DHT11 pulled the voltage low -> wait for it to pull it high again

    // Wait for the DHT11 to pull the line high
    timeout = DHT_TIMEOUT;
	while (GPIO_PinRead(DHT11_GPIO, DHT11_PIN) == 0) {
        if (--timeout == 0) {
        	return 2; // Timeout
        }
        SDK_DelayAtLeastUs(1, CLOCK_GetFreq(kCLOCK_CoreSysClk));
    }
	//DHT11 pulled the voltage high -> wait for low for starting the transmission

    // Wait for the line to go low
    timeout = DHT_TIMEOUT;
	while (GPIO_PinRead(DHT11_GPIO, DHT11_PIN) == 1) {
        if (--timeout == 0) {
        	return 3; // Timeout
        }
        SDK_DelayAtLeastUs(1, CLOCK_GetFreq(kCLOCK_CoreSysClk));
	}
	// The voltage is low -> starting reading data
    // Read 40 bits of data
    for (int i = 0; i < 5; i++) {
    	uint8_t byte = DHT_ReadByte();
    	if (byte == -1) {
    		return 4;
    	}
    	data[i] = byte;
    }

    // Checksum validation
    if ((data[0] + data[1] + data[2] + data[3]) != data[4]) {
        return 6;
    }

    // Extract humidity and temperature
    *humidity = data[0];
    *temperature = data[2];

    return 0;
}

#endif /* SRC_DHT11_C_ */
