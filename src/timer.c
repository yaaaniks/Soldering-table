#include "timer.h"
#include "tempControl.h"
#include "constants.h"

#define TIM3_FREQ   1000U
volatile uint32_t timeStamp, delay;
uint8_t PWM_Fill = 20;

void TIM1_Init(void) {
    RCC->APBENR2 |= RCC_APBENR2_TIM1EN;
    TIM1->SMCR &= ~ TIM_SMCR_SMS;
    TIM1->PSC = 639;
    TIM1->ARR = TIM3_DELAY;
    TIM1->DIER |= TIM_DIER_UIE;
    TIM1->CR1 = TIM_CR1_CEN;
    TIM1->SR = 0;

    NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 2);
    NVIC_EnableIRQ (TIM1_BRK_UP_TRG_COM_IRQn);
}

/**
 * @brief Timer to trigger ADC each 1/TIM3_FREQ sec
 * 
 */
void TIM3_Init(void) {
    RCC->APBRSTR1 |= RCC_APBRSTR1_TIM3RST;
    __DMB();
    RCC->APBRSTR1 &= ~RCC_APBRSTR1_TIM3RST;

    RCC->APBENR1 |= RCC_APBENR1_TIM3EN;
    
    uint32_t apbClk = SystemCoreClock / 2;
    
    uint32_t psc = (apbClk / 1000000) - 1;
	TIM3->PSC = psc;
    
    uint32_t arr = apbClk / ((psc + 1) * TIM3_FREQ) - 1;  
    TIM3->ARR = arr;
    
    TIM3->CR1 &= ~(TIM_CR1_CMS | TIM_CR1_DIR);
    TIM3->CR1 |= TIM_CR1_ARPE;  

    TIM3->CR2 &= ~(TIM_CR2_MMS);
    TIM3->CR2 |= TIM_CR2_MMS_1; 

    TIM3->DIER |= TIM_DIER_UDE;
    
    TIM3->SR = 0;

    TIM3->CR1 |= TIM_CR1_CEN;
    TIM3->EGR |= TIM_EGR_UG;
}

void __attribute__ ((interrupt, used)) TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
    TIM1->SR &= ~TIM_SR_UIF;
} 

void __attribute__ ((interrupt, used)) SysTick_Handler(void) { timeStamp++; }

uint32_t getTimestamp(void) { return timeStamp; }

void delay_ms(uint32_t ms) {
    delay = timeStamp + ms;
    while(delay != timeStamp);
}