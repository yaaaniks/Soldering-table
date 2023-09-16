#include "io.h"

/* Send a char through ITM */
void ITM_SendChar(uint8_t ch) {
    // read FIFO status in bit [0]:
    while(!(ITM_STIMULUS_PORT0 & 1));
    // write to ITM stimulus port0
    ITM_STIMULUS_PORT0 = ch;
}

/* Override low-level _write system call */
int _write(int file, char *ptr, int len) {
    int DataIdx;
    for (DataIdx = 0; DataIdx < len; DataIdx++) {
        ITM_SendChar(*ptr++);
    }
    return len;
}

void GPIO_Init(void) {
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE4);	
    GPIOA->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE4_0);	
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT4);	
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED4);	
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD4);	
    GPIOA->BSRR |= (GPIO_BSRR_BS2 | GPIO_BSRR_BR4);	
}