/**
 * @file rcc.h
 * @author Semikozov Ian (yansemikozov03@icloud.com)
 * @brief configuration of RCC for STM32G031
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef RCC_H
#define RCC_H

#include "stm32g031xx.h"

void SystemClockInit_64MHz(void);
void RCC_DeInit(void);

#endif // RCC_H