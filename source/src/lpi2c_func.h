/*
 * lpi2c_func.h
 *
 *  Created on: Jan 8, 2025
 *      Author: Masha
 */

#ifndef LPI2C_FUNC_H_
#define LPI2C_FUNC_H_

#define LPI2C_MASTER_BASE LPI2C1
#define LPI2C_MASTER_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_Usb1PllClk)
#define LPI2C_BAUDRATE 100000U
#define MCP23017_1_ADDRESS 0x21
#define MCP23017_2_ADDRESS 0x22

enum PinGroup {
	GPA,
	GPB,
};

void LPI2C_Init(void);

status_t LPI2C_WriteData(uint8_t slaveAddress, uint8_t *data, size_t dataSize);

status_t MCP23017_WriteRegister(uint8_t MCP23017Address, uint8_t reg, uint8_t value);

status_t MCP23017_ReadRegister(uint8_t MCP23017Address, uint8_t reg, uint8_t *value);

void MCP23017_SetPin(uint8_t MCP23017Address, int8_t group, uint8_t pin, bool val);

#endif /* LPI2C_FUNC_H_ */
