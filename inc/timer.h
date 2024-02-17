#ifndef TIMER_H
#define TIMER_H

#include "stm32g0xx.h"

#define F_CPU               64000000UL
#define F_CPU_MHZ           F_CPU/1000000
#define TIM3_DELAY          25000UL

void TIM3_Init(void);
void TIM1_Init(void);
void delay_ms(uint32_t ms);
uint32_t getTimestamp(void);
void __attribute__ ((interrupt, used)) SysTick_Handler(void);
void __attribute__ ((interrupt, used)) TIM1_BRK_UP_TRG_COM_IRQHandler(void);

#endif // TIMER_H