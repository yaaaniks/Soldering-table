#include "stm32g031xx.h"
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

void beginScreen() {
	ssd1306_DrawRectangle(1, 1, 127, 63, White);
	ssd1306_SetCursor(6, 15);
    ssd1306_WriteString("Temperature: ", Font_6x8, White);

	ssd1306_DrawCircle(95, 15, 1, White);
    ssd1306_SetCursor(100, 15);
    ssd1306_WriteChar('C', Font_6x8, White);

	ssd1306_SetCursor(6, 30);
	ssd1306_WriteString("User: ", Font_6x8, White);

	ssd1306_DrawCircle(95, 30, 1, White);
    ssd1306_SetCursor(100, 30);
    ssd1306_WriteChar('C', Font_6x8, White);
	ssd1306_UpdateScreen();
}

void drawWallPaper() {
	for (uint8_t i = 0; i < 9; ++i) {
		ssd1306_Fill(Black);
    	ssd1306_DrawBitmap(0, 0, monkeyAnimation[i], 128, 64, White);
		ssd1306_UpdateScreen();
		// delay_ms(4);
	}
}

int main(void) {
	__enable_irq();
    SystemClockInit_64MHz();     
    SysTick_Init();
	ssd1306_Init();
	beginScreen();
	TIM3_Init();
	TIM2_Init();
	ADC1_Init();
	ADC1_Start();
    
	EXTI_Init();
	
	// TIM1_Init();
	Event_t ev;
	__NOP();
	__NOP();
		
    for(;;) {                                             
    }
	return 0;
}