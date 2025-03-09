/*
 * freertos_uart.c
 *
 *  Created on: Feb 3, 2025
 *      Author: Masha
 */

#ifndef TASKS_FREERTOS_UART_C_
#define TASKS_FREERTOS_UART_C_

#include <tasks/wifi_tasks.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include <stdio.h>

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#include "pin_mux.h"
#include "clock_config.h"
#include "ui/manager.h"

QueueHandle_t xQueueWifiCommand;
QueueHandle_t xQueueWifiResponse;

#define UART_BUFFER_SIZE 1024   // Maximum expected response size
#define AT_RESPONSE_TIMEOUT 5000  // Timeout in milliseconds
#define MAX_SSID_LENGTH 32

// Possible AT response endings
const uint8_t AT_OK[] = "\r\nOK\r\n";
const uint8_t AT_ERROR[] = "\r\nERROR\r\n";

uint8_t background_buffer[UART_BUFFER_SIZE];
uint8_t recv_buffer[8];

lpuart_rtos_handle_t handle;
struct _lpuart_handle t_handle;

lpuart_rtos_config_t uart_config = {
    .baudrate = 115200,
    .parity = kLPUART_ParityDisabled,
    .stopbits = kLPUART_OneStopBit,
    .buffer = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

status_t WifiSendCommand(char *to_send) {
	if (0 > LPUART_RTOS_Send(&handle, (uint8_t *)to_send, strlen(to_send)))
	{
	    PRINTF("The command was not send");
	    return kStatus_Fail;
	}
}

int uart_read_byte(uint8_t *byte) {
    size_t n;

	int error = LPUART_RTOS_Receive(&handle, byte, 1, &n);
	if (error == kStatus_Success && n > 0) {
		return 1;
	}
    if (error == kStatus_LPUART_RxHardwareOverrun)
     {
     	PRINTF("\r\nRing buffer overrun!\r\n");
     	return 0;
     }
     if (error == kStatus_LPUART_RxRingBufferOverrun)
     {
     	PRINTF("\r\nHardware buffer overrun!\r\n");
     	return 0;
     }
     return 0;
}

typedef struct {
    char ssid[MAX_SSID_LENGTH];
    int rssi;
} WiFiNetwork;

void GetTopWifiNetworks(char *at_response) {
    WiFiNetwork networks[MAX_WIFI_NETWORKS];
    int count = 0;

    const char *ptr = at_response;
    while ((ptr = strstr(ptr, "+CWLAP:(")) != NULL && count < MAX_WIFI_NETWORKS) {
        int rssi;
        char ssid[MAX_SSID_LENGTH];

        if (sscanf(ptr, "+CWLAP:(\"%31[^\"]\",%d)", ssid, &rssi) > 0) {
            if (strlen(ssid) == 0) {
                 ptr = strstr(ptr + 1, "+CWLAP:");
                 continue;
             }
			strcpy(networks[count].ssid, ssid);
			networks[count].rssi = rssi;
			count++;
        }
        ptr++;
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (networks[j].rssi > networks[i].rssi) {
                WiFiNetwork temp = networks[i];
                networks[i] = networks[j];
                networks[j] = temp;
            }
        }
    }

    at_response[0] = '\0';
    for (int i = 0; i < count; i++) {
        char temp[64];
        snprintf(temp, sizeof(temp), "+CWLAP:(\"%s\")(%d)", networks[i].ssid, networks[i].rssi);
        strncat(at_response, temp, 1023 - strlen(at_response));
    }
}

AT_ResponseStatus WifiReadResponse(char* response) {
    int index = 0;
    uint8_t byte = 0;
    int timeout = 20;
    while(timeout) {
    	if (uart_read_byte(&byte) == 0) {
    		timeout--;
    		continue;
    	}
		response[index] = byte;
		index++;
		if (strstr(response, "\r\nOK\r\n") != NULL) {
			response[index] = '\0';
			PRINTF("%s \r\n ----------------------------- \r\n", response);
			return AT_SUCCESS;
		}
		if (strstr(response, AT_ERROR) != NULL){
			response[index] = '\0';
			PRINTF("%s \r\n ----------------------------- \r\n", response);
			return AT_ERROR_RESPONSE;
		}
    }
	return AT_TIMEOUT;
}

void WifiInit() {
    uart_config.srcclk = BOARD_DebugConsoleSrcFreq();
    uart_config.base = DEMO_UART;
    uart_config.tx_timeout_multiplier_ms = 5;
    uart_config.tx_timeout_constant_ms = 20;
    uart_config.rx_timeout_multiplier_ms = 5;
    uart_config.rx_timeout_constant_ms = 20;

    if (0 > LPUART_RTOS_Init(&handle, &t_handle, &uart_config))
    {
        PRINTF("LPUART_RTOS_Init error");
    }
}

void WifiTask(void *pvParameters) {
	WifiInit();

    char command[128] = {0};
	char long_response[1024] = {0};

	xQueueWifiCommand = xQueueCreate(8, sizeof(command));
	xQueueWifiResponse = xQueueCreate(5, sizeof(long_response));

	while (1) {
		if (xQueueReceive(xQueueWifiCommand, &command, ( TickType_t ) 0)) {
			WifiSendCommand(command);
		}
		memset(long_response, 0, sizeof long_response);
		WifiReadResponse(long_response);
		if (strstr(long_response, "+CWLAP:") != NULL) {
			GetTopWifiNetworks(long_response);
		}

		vTaskDelay(10);
		if (long_response[0] != '\0') {
	        xQueueSend(xQueueWifiResponse, (void*)&long_response, (TickType_t )0);
		}
		vTaskDelay(200);
	}
}

#endif /* TASKS_FREERTOS_UART_C_ */
