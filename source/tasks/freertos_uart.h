/*
 * freertos_uart.h
 *
 *  Created on: Feb 3, 2025
 *      Author: Masha
 */

#ifndef TASKS_FREERTOS_UART_H_
#define TASKS_FREERTOS_UART_H_

#define DEMO_UART LPUART3
#define DEMO_UART_CLKSRC BOARD_DebugConsoleSrcFreq()
#define DEMO_UART_RX_TX_IRQn LPUART3_IRQn
#define uart_task_PRIORITY (configMAX_PRIORITIES - 1)

void uart_task(void *pvParameters);

#endif /* TASKS_FREERTOS_UART_H_ */
