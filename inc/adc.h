#ifndef ADC_H
#define ADC_H

#include "stm32g0xx.h"
#include "tempControl.h"

void adcInit(void);
void __attribute__((interrupt, used)) ADC1_IRQHandler(void);
void __attribute__((interrupt, used)) DMA1_Channel1_IRQHandler();   // FIXME: handler is not called
#endif // ADC_H