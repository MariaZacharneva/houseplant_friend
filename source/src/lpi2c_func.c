/*
 * lpi2c_func.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Masha
 */

#include "FreeRTOS.h"
#include "task.h"

#include "fsl_device_registers.h"
#include "fsl_gpio.h"
#include "fsl_iomuxc.h"
#include "fsl_debug_console.h"
#include "fsl_lpi2c.h"
#include "lvgl_support.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "lvgl.h"
#include "lpi2c_func.h"


#define IODIRA_REG 0x00        // GPIOA direction register
#define OLATA_REG  0x14        // GPIOA output latch register
#define IODIRB_REG 0x01        // GPIOB direction register
#define OLATB_REG  0x15        // GPIOB output latch register

void LPI2C_Init(void) {
    lpi2c_master_config_t masterConfig;

    CLOCK_SetMux(kCLOCK_Lpi2cMux, 0);
    CLOCK_SetDiv(kCLOCK_Lpi2cDiv, 1);

    LPI2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Hz = LPI2C_BAUDRATE;

    LPI2C_MasterInit(LPI2C_MASTER_BASE, &masterConfig, LPI2C_MASTER_CLOCK_FREQ);
}

status_t LPI2C_WriteData(uint8_t slaveAddress, uint8_t *data, size_t dataSize) {
    status_t status = LPI2C_MasterStart(LPI2C_MASTER_BASE, slaveAddress, kLPI2C_Write);
    if (status != kStatus_Success) {
        return status;
    }

    LPI2C_MASTER_BASE->MCR |= LPI2C_MCR_DBGEN_MASK;
    status = LPI2C_MasterSend(LPI2C_MASTER_BASE, data, dataSize);
    if (status != kStatus_Success) {
        return status;
    }

    status = LPI2C_MasterStop(LPI2C_MASTER_BASE);
    return status;
}

status_t MCP23017_WriteRegister(uint8_t MCP23017Address, uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    return LPI2C_WriteData(MCP23017Address, data, sizeof(data));
}

status_t LPI2C_ReadData(uint8_t MCP23017Address, uint8_t reg, uint8_t *value) {
    status_t status;

    // Step 1: Send the register address
    status = LPI2C_MasterStart(LPI2C_MASTER_BASE, MCP23017Address, kLPI2C_Write);
    if (status != kStatus_Success) {
        PRINTF("Error in MasterStart: %d\r\n", status);
        return status;
    }

    LPI2C_MASTER_BASE->MCR |= LPI2C_MCR_DBGEN_MASK;
    status = LPI2C_MasterSend(LPI2C_MASTER_BASE, &reg, 1);
    if (status != kStatus_Success) {
        PRINTF("Error in MasterSend: %d\r\n", status);
        LPI2C_MasterStop(LPI2C_MASTER_BASE);
        return status;
    }

    // Step 2: Issue a repeated start and switch to read mode
    status = LPI2C_MasterRepeatedStart(LPI2C_MASTER_BASE, MCP23017Address, kLPI2C_Read);
    if (status != kStatus_Success) {
        PRINTF("Error in RepeatedStart: %d\r\n", status);
        LPI2C_MasterStop(LPI2C_MASTER_BASE);
        return status;
    }

    // Step 3: Receive a single byte of data
    status = LPI2C_MasterReceive(LPI2C_MASTER_BASE, value, 1);
    if (status != kStatus_Success) {
        PRINTF("Error in MasterReceive: %d\r\n", status);
        LPI2C_MasterStop(LPI2C_MASTER_BASE);
        return status;
    }

    // Step 4: Send a stop condition
    status = LPI2C_MasterStop(LPI2C_MASTER_BASE);
    if (status != kStatus_Success) {
        PRINTF("Error in MasterStop: %d\r\n", status);
    }

    return status;
}

status_t MCP23017_ReadRegister(uint8_t MCP23017Address, uint8_t reg, uint8_t *value) {
    return LPI2C_ReadData(MCP23017Address, reg, value);
}

status_t MCP23017_SetPin(uint8_t MCP23017Address, int8_t group, uint8_t pin, bool val) {
    uint8_t value;
    status_t status;

    uint8_t IODIR;
    uint8_t OLAT;

    if (group == GPA) {
    	IODIR = IODIRA_REG;
    	OLAT = OLATA_REG;
    } else {
    	IODIR = IODIRB_REG;
    	OLAT = OLATB_REG;
    }

    // Step 1: Set the pin direction to OUTPUT (IODIRA)
    status = MCP23017_ReadRegister(MCP23017Address, IODIR, &value);
    if (status != kStatus_Success) {
    	PRINTF("Error in MCP23017_ReadRegister: %d\r\n", status);
    	return status;
    }
    value &= ~(1 << pin);
    status = MCP23017_WriteRegister(MCP23017Address, IODIR, value);
    if (status != kStatus_Success) {
    	PRINTF("Error in MCP23017_WriteRegister: %d\r\n", status);
    	return status;
    }

    // Step 2: Set the pin HIGH (OLATA)
    status = MCP23017_ReadRegister(MCP23017Address, OLAT, &value);
    if (status != kStatus_Success) {
    	PRINTF("Error in MCP23017_ReadRegister: %d\r\n", status);
    	return status;
    }

    if (val == 1) {
        value |= (1 << pin);
    } else {
        value &= ~(1 << pin);
    }
    status = MCP23017_WriteRegister(MCP23017Address, OLAT, value);
    if (status != kStatus_Success) {
    	PRINTF("Error in MCP23017_WriteRegister: %d\r\n", status);
    	return status;
    }
    return kStatus_Success;
}
