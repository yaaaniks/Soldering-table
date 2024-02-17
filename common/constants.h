#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "stm32g0xx.h"

#define ADC_BUFFER_SIZE     64
#define ST_NUM				5
#define EV_NUM				3
#define MAX_RAW_VALUE       1638

extern uint16_t userTemperature;
extern uint16_t currentTemperature;

#endif // CONSTANTS_H