/*
 * keyboard1.c
 *
 *  Created on: Feb 26, 2025
 *      Author: Masha
 */
#include "keyboard.h"

#include "fsl_lpi2c.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdbool.h>
#include "src/lpi2c_func.h"

#define IODIRA_REG 0x00        // GPIOA direction register
#define OLATA_REG  0x14        // GPIOA output latch register
#define IODIRB_REG 0x01        // GPIOB direction register
#define OLATB_REG  0x15        // GPIOB output latch register
#define GPIOA_REG  0x12  		// GPIOA input register (columns)

const char key_map[4][4][5] = {
    {"1", "ABC2", "DEF3", "+"},
    {"GHI4", "JKL5", "MNO6", "-"},
    {"PQRS7", "TUV8", "WXYZ9", "*"},
    {"[", "0", "]", "#"}
};

int ROW, COL;

void MCP23017_Setup_Keypad() {
    MCP23017_WriteRegister(MCP23017_2_ADDRESS, IODIRB_REG, 0x00);
    MCP23017_WriteRegister(MCP23017_2_ADDRESS, OLATB_REG, 0x00);
    MCP23017_WriteRegister(MCP23017_2_ADDRESS, IODIRA_REG, 0xFF);
    MCP23017_WriteRegister(MCP23017_2_ADDRESS, 0x0C, 0x00);
}


char MCP23017_ScanKeypad() {
    uint8_t value = 0;
    MCP23017_WriteRegister(MCP23017_2_ADDRESS, OLATA_REG, 0x00);

    for (int row = 0; row < 4; row++) {
    	MCP23017_SetPin(MCP23017_2_ADDRESS, GPB, row, 1);

    	MCP23017_ReadRegister(MCP23017_2_ADDRESS, GPIOA_REG, &value);
        for (int col = 0; col < 4; col++) {
            while ((value & (1 << col))) {
            	while (value != 0) {
                    MCP23017_ReadRegister(MCP23017_2_ADDRESS, GPIOA_REG, &value);
            	}
            	MCP23017_SetPin(MCP23017_2_ADDRESS, GPB, row, 0);
            	ROW = 3 - row;
            	COL = col;
                return key_map[ROW][COL][0];
            }
        }
    	MCP23017_SetPin(MCP23017_2_ADDRESS, GPB, row, 0);
    }
    return '\0';  // No key pressed
}


char last_key = '\0';
int press_count = 0;
TickType_t last_press_time = 0;

char Get_T9_Character(char key) {
    TickType_t current_time = xTaskGetTickCount();

    if (key == last_key) {
        if (current_time - last_press_time < pdMS_TO_TICKS(1000)) {
            press_count = (press_count + 1) % strlen(key_map[ROW][COL]);
        } else {
            press_count = 0;
        }
    } else {
        press_count = 0;
    }

    last_key = key;
    last_press_time = current_time;
    return key_map[ROW][COL][press_count];
}

void KeyboardInputTask(void *param) {
	MCP23017_Setup_Keypad();
    char received_char;
    char buffer[32] = "";

    while (1) {
        received_char = MCP23017_ScanKeypad();

        if (received_char != '\0') {
            received_char = Get_T9_Character(received_char);

            PRINTF("%c", received_char);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}


