#ifndef ADC_H
#define ADC_H

#include "stm32g0xx.h"
#include "tempControl.h"
#include "fsm.h"

/* low pass filter with 8 bit resolution, p = 0..255 */
#define LOW_PASS_BITS 8
/* number of ADC sources, which will be scanned and copied to ADCRawValues */
#define ADC_SRC_CNT 64

extern volatile uint16_t ADCRawValues[ADC_SRC_CNT];
void adcInit(void);
void __attribute__((interrupt, used)) ADC1_IRQHandler(void);
void __attribute__((interrupt, used)) DMA1_Channel1_IRQHandler();   // FIXME: handler is not called
#endif // ADC_H