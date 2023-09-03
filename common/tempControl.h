#ifndef TEMP_CONTROL_H
#define TEMP_CONTROL_H

#include <stdio.h>
#include <string.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "constants.h"
#include "fsm.h"
#include "timer.h"

void updateTemp(volatile uint16_t *adcData);
void updateScreen(void);

#endif // TEMP_CONTROL_H
