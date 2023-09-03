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

void heartBeatLedInit()
{
	GPIOA->MODER &= ~GPIO_MODER_MODE4;	
    GPIOA->MODER |= GPIO_MODER_MODE4_0;	
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT4;	
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED4;	
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD4;	
    GPIOA->BSRR = GPIO_BSRR_BR4;
}

Fsm_Cell_t fsmTable[] = {
	{ST_UPDATESCREEN, ST_STARTUP, EV_ADC},
	{ST_UPDATESCREEN, ST_UPDATESCREEN, EV_ADC},
	{ST_UPDATESCREEN, ST_UPDATESCREEN, EV_NONE}
};

void (*stateHandler[])() = {
	[ST_STARTUP] = updateScreen,
	[ST_UPDATESCREEN] = updateScreen
};

size_t numCells = sizeof(fsmTable) / sizeof(fsmTable[0]);

int main() {
	__enable_irq();
    SystemClockInit_64MHz();     
    SysTick_Init();
    I2C2_Config();
	TIM3_Init();
	ADC1_Init();
	ADC1_Start();
    ssd1306_Init();
	EXTI_Init();
    // ssd1306_TestDrawBitmap();
	Event_t ev;
	__NOP();
	__NOP();
	fsmInit(&fsm, &fsmTable, numCells, &stateHandler, ST_NUM);		
    for(;;){
		ev = fifoPull(&fsm);
		fsmDispatch(&fsm, ev);
        // delay_ms(500);
        // GPIOA->BSRR = GPIO_BSRR_BS4;		/* atomic set PA3 */
        // delay_ms(500);
        // GPIOA->BSRR = GPIO_BSRR_BR4;		/* atomic clr PA3 */
    }
}