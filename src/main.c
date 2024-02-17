#include "stm32g0xx.h"
#include "timer.h"
#include "fsm.h"
#include "i2c.h"
#include "adc.h"
#include "ssd1306.h"
#include "ssd1306_tests.h"
#include "tempControl.h"
#include "constants.h"
#include "exti.h"
#include "bitmap.h"

void infoFrame() {
	ssd1306_DrawRectangle(1, 1, 127, 63, White);
	ssd1306_SetCursor(6, 15);
    ssd1306_WriteString("Temperature: ", Font_6x8, White);

	ssd1306_DrawCircle(105, 15, 1, White);
    ssd1306_SetCursor(110, 15);
    ssd1306_WriteChar('C', Font_6x8, White);

	ssd1306_SetCursor(6, 30);
	ssd1306_WriteString("User: ", Font_6x8, White);

	ssd1306_DrawCircle(105, 30, 1, White);
    ssd1306_SetCursor(110, 30);
    ssd1306_WriteChar('C', Font_6x8, White);
	ssd1306_UpdateScreen();
}

void drawWallPaper() {
	for (uint8_t i = 0; i < 9; ++i) {
		ssd1306_Fill(Black);
    	ssd1306_DrawBitmap(0, 0, monkeyAnimation[i], 128, 64, White);
		ssd1306_UpdateScreen();
		delay_ms(20);
	}
}

int main(void) {
    SystemClockInit_64MHz();     
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE4);	
    GPIOA->MODER |= (GPIO_MODER_MODE2_0 | GPIO_MODER_MODE4_0);	
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT2 | GPIO_OTYPER_OT4);	
    GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED2 | GPIO_OSPEEDR_OSPEED4);	
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD2 | GPIO_PUPDR_PUPD4);	
    GPIOA->BSRR |= (GPIO_BSRR_BS2 | GPIO_BSRR_BR4);	
	
	// i2cInit();
	// ssd1306_Init();
	// infoFrame();
	
	TIM3_Init();

	adcInit();
    
	EXTI_Init();
	__enable_irq();
	__NOP();
	__NOP();
    for(;;) {
		// drawWallPaper();
		// delay_ms(500);
    	// GPIOA->BSRR = GPIO_BSRR_BS4;		
    	// delay_ms(500);
    	// GPIOA->BSRR = GPIO_BSRR_BR4;                                         
    }
	return 0;
}