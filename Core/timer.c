#include "timer.h"

volatile uint32_t SysTick_CNT = 0;
volatile uint32_t timeOut = 0;

void SysTick_Init(void) {
    SystemCoreClockUpdate();                         
    SysTick_Config(SystemCoreClock/1000-1); // set tick to every 1ms	
}

void TIM3_Init(void) {
    RCC->APBENR1 |= RCC_APBENR1_TIM3EN; //enable clock for timer3
	TIM3->PSC = F_CPU_MHZ - 1;   
    // TIM3->CR1 |= TIM_CR1_ARPE;		//enable auto reload
	// TIM3->CR1 |= TIM_CR1_DIR;		//direction = down-counter  
    TIM3->CR2 &= ~(TIM_CR2_MMS);
    // TIM3->CR2 |= TIM_CR2_MMS_1;            // output (TRGO)
    TIM3->DIER |= TIM_DIER_UDE;
    // TIM3->DIER |= TIM_DIER_UIE;
    // TIM3->BDTR |= TIM_BDTR_MOE;
    TIM3->SR = 0;
    TIM3->ARR = TIM3_DELAY;
    TIM3->CR1 |= TIM_CR1_CEN;
    NVIC_SetPriority(TIM3_IRQn, 2);
    NVIC_EnableIRQ(TIM3_IRQn);
}


void __attribute__ ((interrupt, used)) TIM3_IRQHandler() {
	TIM3->SR = 0;	// clear pending bit in the interrupt handler
}

void __attribute__ ((interrupt, used)) SysTick_Handler(void) {
    if (SysTick_CNT > 0) SysTick_CNT--;
	if (timeOut != 0) 
		timeOut--;
}

void delay_ms(uint32_t ms) {
    SysTick_CNT = ms;
    while(SysTick_CNT); // rollover-safe (within limits)
}