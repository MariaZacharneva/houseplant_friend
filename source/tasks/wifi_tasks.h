/*
 * freertos_uart.h
 *
 *  Created on: Feb 3, 2025
 *      Author: Masha
 */

#ifndef TASKS_FREERTOS_UART_H_
#define TASKS_FREERTOS_UART_H_

#include "FreeRTOS.h"
#include "queue.h"

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#define DEMO_UART LPUART3
#define DEMO_UART_CLKSRC BOARD_DebugConsoleSrcFreq()
#define DEMO_UART_RX_TX_IRQn LPUART3_IRQn
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)


extern QueueHandle_t xQueueWifiCommand;
extern QueueHandle_t xQueueWifiResponse;

typedef enum {
    AT_SUCCESS,
    AT_ERROR_RESPONSE,
    AT_TIMEOUT,
    AT_UNKNOWN,
	AT_MEM_LIMIT,
} AT_ResponseStatus;

void WifiInit();

AT_ResponseStatus WifiReadResponse(char* response);

status_t WifiSendCommand(char* to_send);

void WifiTask(void *pvParameters);

#endif /* TASKS_FREERTOS_UART_H_ */
