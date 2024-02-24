#include "stm32g0xx.h"
#include "timer.h"
#include "i2c.h"
#include "adc.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "tempControl.h"
#include "constants.h"
#include "menu.h"
#include "io.h"

int main(void) {
    systemClockInit_64MHz();     
	
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE4);	
    GPIOA->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE4_0);	
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT4);	
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED4);	
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD4);	
    GPIOA->BSRR |= (GPIO_BSRR_BS2 | GPIO_BSRR_BR4);	
	
	i2cInit();
	menuInit();
	adcInit();
    
	ioInit();
	__enable_irq();
    for(;;) {
		delay_ms(500);
    	GPIOA->BSRR = GPIO_BSRR_BS4;		
    	delay_ms(500);
    	GPIOA->BSRR = GPIO_BSRR_BR4;                                         
    }
	return 0;
}