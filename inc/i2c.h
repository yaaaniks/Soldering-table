#ifndef I2C_H
#define I2C_H

#include <stddef.h> 

#include "stm32g0xx.h"

void i2cInit(void);
void i2cTransmit(const uint8_t devAddr, const uint8_t memAddr, const uint8_t* data, size_t size);
void i2cRead(const uint8_t devAddr, uint8_t* data, size_t size);

#endif // I2C_H