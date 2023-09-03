#ifndef CONSTANTS_H
#define CONSTANTS_H

#include "stm32g031xx.h"


#define SCB_AIRCR			(*(unsigned volatile long*)0xE000ED0C)      // acces to SCB_AIRCR
#define SCB_AIRCR_GROUP22	0x05FA0500                                  // change priority data

#define ADC_BUFFER_SIZE     64
#define ST_NUM				3
#define EV_NUM				2
#define ST_H_NUM            2
#define MAX_RAW_VALUE       1638

extern uint16_t userTemperature;
extern float currentTemperature;

#endif // CONSTANTS_H