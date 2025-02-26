/*
 * Copyright 2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "fsl_lpuart.h"
#include "lvgl_support.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "lvgl.h"

#include "src/common.h"
#include "src/lpi2c_func.h"

#include "tasks/lvgl_tasks.h"
#include "tasks/pump_tasks.h"
#include "tasks/sensor_tasks.h"
#include "tasks/dht11_tasks.h"
#include "tasks/freertos_uart.h"
#include "tasks/keyboard.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/



/*******************************************************************************
 * Prototypes
 ******************************************************************************/
#if LV_USE_LOG
static void print_cb(const char *buf)
{
    PRINTF("\r%s\n", buf);
}
#endif


/*******************************************************************************
 * Code
 ******************************************************************************/
AT_QUICKACCESS_SECTION_CODE(void BOARD_ReconfigFlexSpiRxBuffer(void));

/*
 * When PXP fetch images from FlexSPI flash, the default FlexSPI RX buffer
 * configuration does not meet the PXP bandwidth requirement. Reconfigure
 * here.
 */
void BOARD_ReconfigFlexSpiRxBuffer(void)
{
    uint32_t ahbcr;

    /* Disable I cache and D cache */
    if (SCB_CCR_IC_Msk == (SCB_CCR_IC_Msk & SCB->CCR))
    {
        SCB_DisableICache();
    }

    if (SCB_CCR_DC_Msk == (SCB_CCR_DC_Msk & SCB->CCR))
    {
        SCB_DisableDCache();
    }

    ahbcr = FLEXSPI2->AHBCR;

    /* Temporarily disable prefetching while changing the buffer settings */
    FLEXSPI2->AHBCR = ahbcr & ~(FLEXSPI_AHBCR_CACHABLEEN_MASK | FLEXSPI_AHBCR_PREFETCHEN_MASK);

    /* Wait for FlexSPI idle to make sure no flash data transfer. */
    while ((FLEXSPI2->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) == 0U)
    {
    }

    /* Allocate half of the prefetch buffer to the core */
    FLEXSPI2->AHBRXBUFCR0[0] =
        FLEXSPI_AHBRXBUFCR0_PREFETCHEN_MASK | FLEXSPI_AHBRXBUFCR0_MSTRID(0) | FLEXSPI_AHBRXBUFCR0_BUFSZ(0x40);

    /* Disable dedicate prefetch buffer for DMA. */
    FLEXSPI2->AHBRXBUFCR0[1] =
        FLEXSPI_AHBRXBUFCR0_PREFETCHEN_MASK | FLEXSPI_AHBRXBUFCR0_MSTRID(1) | FLEXSPI_AHBRXBUFCR0_BUFSZ(0x00);

    /* Disable dedicate prefetch buffer for DCP. */
    FLEXSPI2->AHBRXBUFCR0[2] =
        FLEXSPI_AHBRXBUFCR0_PREFETCHEN_MASK | FLEXSPI_AHBRXBUFCR0_MSTRID(2) | FLEXSPI_AHBRXBUFCR0_BUFSZ(0x00);

    /* Other half of the buffer for other masters incl. PXP */
    FLEXSPI2->AHBRXBUFCR0[3] =
        FLEXSPI_AHBRXBUFCR0_PREFETCHEN_MASK | FLEXSPI_AHBRXBUFCR0_MSTRID(3) | FLEXSPI_AHBRXBUFCR0_BUFSZ(0x40);

    FLEXSPI2->AHBCR = ahbcr; /* Set AHBCR back to the original value */

    /* Enable I cache and D cache */
    SCB_EnableDCache();
    SCB_EnableICache();
}



/*!
 * @brief Main function
 */
int main(void) {

    *((volatile uint32_t *)0x41044100) = 5;

    BOARD_ConfigMPU();
    BOARD_ReconfigFlexSpiRxBuffer();
    BOARD_InitPins();
    BOARD_InitI2C1Pins();
    BOARD_InitSemcPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    LPI2C_Init();
    CustomADC_Init();
    Plants_Init();

    NVIC_SetPriority(DEMO_UART_RX_TX_IRQn, 5);
    BaseType_t display_stat = pdPASS, sensor_stat = pdPASS,
    		pump_stat = pdPASS, dht_stat = pdPASS, uart_stat = pdPASS, keyboard_stat = pdPASS;

	display_stat = xTaskCreate(DisplayTask, "display", configMINIMAL_STACK_SIZE + 800, NULL, tskIDLE_PRIORITY + 2, NULL);

    sensor_stat = xTaskCreate(UpdateSensorsTask, "sensor", configMINIMAL_STACK_SIZE + 50, NULL, tskIDLE_PRIORITY + 3, NULL);

    pump_stat = xTaskCreate(PumpControllerTask, "pump", configMINIMAL_STACK_SIZE + 50, NULL, tskIDLE_PRIORITY + 2, NULL);

	dht_stat = xTaskCreate(DHTTask, "dht", configMINIMAL_STACK_SIZE + 50, NULL, tskIDLE_PRIORITY + 3, NULL);

    uart_stat =  xTaskCreate(uart_task, "uart", configMINIMAL_STACK_SIZE + 200, NULL, tskIDLE_PRIORITY + 3, NULL);

    keyboard_stat =  xTaskCreate(KeyboardInputTask, "keyboard", configMINIMAL_STACK_SIZE + 200, NULL, tskIDLE_PRIORITY + 3, NULL);

    if ((display_stat != pdPASS) ||(sensor_stat != pdPASS) || (pump_stat != pdPASS) ||
    		(dht_stat != pdPASS) || (uart_stat != pdPASS)) {
		PRINTF("Failed to create a task");
		while (true) {}
    }

    vTaskStartScheduler();

    while (true) {} /* should never get here */
}

/*!
 * @brief Malloc failed hook.
 */
void vApplicationMallocFailedHook(void)
{
    PRINTF("Malloc failed. Increase the heap size.");

    while (true){}
}

/*!
 * @brief FreeRTOS tick hook.
 */
void vApplicationTickHook(void)
{
    if (s_lvgl_initialized)
    {
        lv_tick_inc(1);
    }
}

/*!
 * @brief Stack overflow hook.
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)pcTaskName;
    (void)xTask;

    while (true) {}
}
