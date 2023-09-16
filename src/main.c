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
#include "io.h"

Fsm_Cell_t fsmTable[] = {
	{ST_UPDATESCREEN, ST_STARTUP, EV_ADC},
	{ST_UPDATESCREEN, ST_UPDATESCREEN, EV_ADC},
	{ST_REGULATOR, ST_UPDATESCREEN, EV_USER},
	{ST_REGULATOR, ST_REGULATOR, EV_ADC},
	{ST_UPDATESCREEN, ST_REGULATOR, EV_USER},
	{ST_UPDATESCREEN, ST_UPDATESCREEN, EV_NONE}
};

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
	// ssd1306_timeUpdate()
}

void drawWallPaper() {
	for (uint8_t i = 0; i < 9; ++i) {
		ssd1306_Fill(Black);
    	ssd1306_DrawBitmap(0, 0, monkeyAnimation[i], 128, 64, White);
		ssd1306_UpdateScreen();
		// delay_ms(4);
	}
}

void wait(void) {
	__NOP();
}

void (*stateHandler[])() = {
	[ST_STARTUP] = updateScreen,
	[ST_USER] = updateScreen,
	[ST_UPDATESCREEN] = updateScreen,
	[ST_NONE] = drawWallPaper
};

        // delay_ms(500);
        // GPIOA->BSRR = GPIO_BSRR_BS2;		/* atomic set PA3 */
        // delay_ms(500);
        // GPIOA->BSRR = GPIO_BSRR_BR2;		/* atomic clr PA3 */

size_t numCells = sizeof(fsmTable) / sizeof(fsmTable[0]);

int main(void) {
	__enable_irq();
    SystemClockInit_64MHz();     
    SysTick_Init();
    I2C2_Config();
	ssd1306_Init();
	beginScreen();
	TIM3_Init();
	TIM2_Init();
	ADC1_Init();
	ADC1_Start();
    
	EXTI_Init();
	GPIO_Init();
	
	// TIM1_Init();
	Event_t ev;
	__NOP();
	__NOP();

	fsmInit(&fsm, &fsmTable, numCells, &stateHandler, ST_NUM);		
    for(;;) {
		ev = fifoPull(&fsm);
		fsmDispatch(&fsm, ev);
		TIM2->CCR2 = PWM_Fill;
    	// // delay_ms(500);                                                      
    	if(PWM_Fill == 90)                                                
      		PWM_Fill = 10;                                                  
    	else
      		PWM_Fill += 10;                                                 
    }
	return 0;
}