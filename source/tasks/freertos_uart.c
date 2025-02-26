/*
 * freertos_uart.c
 *
 *  Created on: Feb 3, 2025
 *      Author: Masha
 */

#ifndef TASKS_FREERTOS_UART_C_
#define TASKS_FREERTOS_UART_C_

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpuart_freertos.h"
#include "fsl_lpuart.h"

#include "pin_mux.h"
#include "clock_config.h"

#include "freertos_uart.h"

#define UART_BUFFER_SIZE 256   // Maximum expected response size
#define AT_RESPONSE_TIMEOUT 5000  // Timeout in milliseconds

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

typedef enum {
    AT_SUCCESS,
    AT_ERROR_RESPONSE,
    AT_TIMEOUT,
    AT_UNKNOWN
} AT_ResponseStatus;

status_t uart_send_command(uint8_t *to_send) {
	if (0 > LPUART_RTOS_Send(&handle, (uint8_t *)to_send, strlen(to_send)))
	{
	    PRINTF("The command was not send");
	    return kStatus_Fail;
		vTaskSuspend(NULL);
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

status_t uart_read_response() {
    uint8_t response[256] = {0};
    int index = 0;
    uint8_t byte;
    while(1) {
    	uart_read_byte(&byte);
		response[index] = byte;
		index++;
		if (strstr(response, AT_OK)) {
			response[index + 1] = '\0';
			PRINTF(response);
			return AT_SUCCESS;
		}
		if (strstr(response, AT_ERROR)){
			response[index + 1] = '\0';
			PRINTF(response);
			return AT_ERROR_RESPONSE;
		}
        vTaskDelay(10);
    }
}

void uart_task(void *pvParameters)
{
    uart_config.srcclk = BOARD_DebugConsoleSrcFreq();
    uart_config.base = DEMO_UART;

    if (0 > LPUART_RTOS_Init(&handle, &t_handle, &uart_config))
    {
        vTaskSuspend(NULL);
    }
    int index = 0;
	uint8_t AT_command[] = 		"AT\r\n";
	uint8_t GMR_command[] = 	"AT+GMR\r\n";\
	uint8_t DEF_command[] = 	"AT+UART_DEF?\r\n";
	uint8_t CWI_command[] = 	"AT+CWINIT?\r\n";
	uint8_t CMODE_command[] =  	"AT+CWMODE=1\r\n";
	uint8_t CWState_command[] =	"AT+CWSTATE?\r\n";
	uint8_t CWJAP_command[] = 	"AT+CWJAP=\"5G-GGL-GANG\",\"meowmeow\"\r\n";


	while (1) {
		switch(index){
			case 0:
				uart_send_command(AT_command);
				index++;
				break;
			case 1:
				uart_send_command(GMR_command);
				index++;
				break;
			case 2:
				uart_send_command(DEF_command);
				index++;
				break;
			case 3:
				uart_send_command(CWI_command);
				index++;
				break;
			case 4:
				uart_send_command(CMODE_command);
				index++;
				break;
			case 5:
				uart_send_command(CWState_command);
				index++;
				break;
			case 6:
				uart_send_command(CWJAP_command);
				index++;
				break;
			default:
				uart_send_command(AT_command);
		}
		uart_read_response();
		PrintStackInfo("uart");

		vTaskDelay(2000);
	}
}

#endif /* TASKS_FREERTOS_UART_C_ */
