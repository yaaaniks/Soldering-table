#include "exti.h"


void EXTI_Init(void) {
    /********** PA7 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE6_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6_1;
    
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD6_0;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD6_1;
    
    EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI6;     // clear EXTI for PA7 - 7(EXTI7)
    // EXTI->EXTICR[1] |= EXTI_EXTICR2_EXTI7_0;   // set EXTI for PA7 - 7(EXTI7)
    EXTI->FTSR1 |= EXTI_FTSR1_FT6;              // set EXTI Rising triger for Line no.7
    EXTI->IMR1 |= EXTI_IMR1_IM6;                // set EXTI Interrupt no-masked for Line no.7 

    /********** PA7 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE7_0 | GPIO_MODER_MODE7_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED7_1;
    
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD7_0;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD7_1;
    
    EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI7;     // clear EXTI for PA7 - 7(EXTI7)
    // EXTI->EXTICR[1] |= EXTI_EXTICR2_EXTI7_0;   // set EXTI for PA7 - 7(EXTI7)
    EXTI->FTSR1 |= EXTI_FTSR1_FT7;              // set EXTI Rising triger for Line no.7
    EXTI->IMR1 |= EXTI_IMR1_IM7;                // set EXTI Interrupt no-masked for Line no.7 

    /********** PA8 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE8_0 | GPIO_MODER_MODE8_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_0;
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_1;
    
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD8_1;
    
    EXTI->EXTICR[2] &= ~EXTI_EXTICR3_EXTI8 ;    // clear EXTI for PA8 - 8(EXTI8) 
    // EXTI->EXTICR[2] |=  EXTI_EXTICR3_EXTI8_0 ;  // set EXTI for PA8 - 8(EXTI8) 
    EXTI->FTSR1 |= EXTI_FTSR1_FT8;              // set EXTI Falling triger for Line no.8 
    EXTI->IMR1 |= EXTI_IMR1_IM8;                // set EXTI Interrupt no-masked for Line no.8 

    NVIC_SetPriority(EXTI4_15_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void __attribute__((interrupt, used)) EXTI4_15_IRQHandler(void) {
    if (EXTI->FPR1 & EXTI_FPR1_FPIF8) {     // interrupt pending from Line no.9 (Falling) ?
        EXTI->FPR1 |= EXTI_FPR1_FPIF8;      // clear pending
        userTemperature++;
    } else if (EXTI->FPR1 & EXTI_FPR1_FPIF7) {      // interrupt pending from Line no.9 (Falling) ?
        EXTI->FPR1 |= EXTI_FPR1_FPIF7;              // clear pending
        userTemperature--;
    } else if (EXTI->FPR1 & EXTI_FPR1_FPIF6) {
        EXTI->FPR1 |= EXTI_FPR1_FPIF7;
    }
}