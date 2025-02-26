/*
 * keyboard.h
 *
 *  Created on: Feb 26, 2025
 *      Author: Masha
 */

#ifndef TASKS_KEYBOARD_H_
#define TASKS_KEYBOARD_H_

#include "fsl_lpi2c.h"
#include "fsl_debug_console.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdbool.h>
#include "src/lpi2c_func.h"

void MCP23017_Setup_Keypad();

char MCP23017_ScanKeypad();

char Get_T9_Character(char key);

void KeyboardInputTask(void *param) ;

#endif /* TASKS_KEYBOARD_H_ */
