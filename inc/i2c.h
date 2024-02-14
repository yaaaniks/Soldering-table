/**
 * @file i2c.h
 * @author Semikozov Ian (yansemikozov03@icloud.com)
 * @brief configuration of I2C for STM32F401CCU6
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef I2C_H
#define I2C_H

#include <stddef.h> 

#include "stm32g0xx.h"

#define APB1_CLK_SPEED      64000000U
#define APB1_CLK_SPEED_MHz  (APB1_CLK_SPEED / 1000000U)
#define I2C1_CLK_SPEED      100000U

void i2cInit(void);
void i2cWrite(const uint8_t devAddr, const uint8_t* data, uint32_t size, uint32_t timeout);
void i2cRead(const uint8_t devAddr, uint8_t* data, uint32_t size, uint32_t timeout);
void i2cMemWrite(const uint8_t devAddr, const uint16_t memAddr, const uint8_t* data, uint32_t size, uint32_t timeout);
void i2cMemRead(const uint8_t devAddr, uint16_t memAddr, uint8_t* data, uint32_t size);

#endif // I2C_H