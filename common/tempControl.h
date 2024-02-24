#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H


#include <stdio.h>
#include <string.h>
#include <math.h>

#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "constants.h"
#include "timer.h"

#define TEMPERATURE_UNDER 0
#define TEMPERATURE_OVER 9999
#define TEMPERATURE_TABLE_START 0
#define TEMPERATURE_TABLE_STEP 0.5
#define TEMPERATURE_TABLE_READ(i) adcDiscreteTable[i]

void updateTemp(volatile uint16_t *adcData);
void updateScreen(void);

#endif // TEMP_CONTROL_H
