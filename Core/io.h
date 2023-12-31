#ifndef IO_H
#define IO_H

#include "stm32g031xx.h"

/* Debug Exception and Monitor Control Register base address */
#define DEMCR                 *((volatile uint32_t*) 0xE000EDFCu)

/* ITM register addresses */
#define ITM_STIMULUS_PORT0    *((volatile uint32_t*) 0xE0000000u)
#define ITM_TRACE_EN          *((volatile uint32_t*) 0xE0000E00u)
#define TTP223_PORT     GPIOA->IDR


typedef __IO uint32_t  vu32;
typedef uint32_t  u32;

void ITM_SendChar(uint8_t ch);
/* Override low-level _write system call */
int _write(int file, char *ptr, int len);
void GPIO_Init(void);

#endif // IO_H
