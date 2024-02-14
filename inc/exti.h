#ifndef EXTI_H
#define EXTI_H

#include "stm32g031xx.h"
#include "constants.h"
#include "fsm.h"

/**
 * @brief 
 * 
 */
void EXTI_Init(void);

/**
 * @brief Handling Interrupt from Line 4..15
 * 
 */
void __attribute__((interrupt, used)) EXTI4_15_IRQHandler(void);

#endif // EXTI_H