#ifndef IO_H
#define IO_H

#include <stdint.h>
#include <stddef.h>

#include "stm32g0xx.h"

void ioInit(void);
void __attribute__((interrupt, used)) EXTI4_15_IRQHandler(void);


#endif // IO_H