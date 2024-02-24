#include "io.h"
#include "menu.h"

void ioInit(void) {
    /********** PA7 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE6_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED6_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED6_1;
    
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD6_0;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD6_1;
    
    EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI6;     
    // EXTI->EXTICR[1] |= EXTI_EXTICR2_EXTI7_0; 
    EXTI->FTSR1 |= EXTI_FTSR1_FT6;              
    EXTI->IMR1 |= EXTI_IMR1_IM6;                

    /********** PA7 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE7_0 | GPIO_MODER_MODE7_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED7_0;
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED7_1;
    
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD7_0;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD7_1;
    
    EXTI->EXTICR[1] &= ~EXTI_EXTICR2_EXTI7;     
    // EXTI->EXTICR[1] |= EXTI_EXTICR2_EXTI7_0; 
    EXTI->FTSR1 |= EXTI_FTSR1_FT7;              
    EXTI->IMR1 |= EXTI_IMR1_IM7;                 

    /********** PA8 set for EXTI ************/
    GPIOA->MODER &= ~(GPIO_MODER_MODE8_0 | GPIO_MODER_MODE8_1); 

    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8_0;
    GPIOB->OSPEEDR &= ~GPIO_OSPEEDR_OSPEED8_1;
    
    GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD8_1;
    
    EXTI->EXTICR[2] &= ~EXTI_EXTICR3_EXTI8 ;    
    // EXTI->EXTICR[2] |=  EXTI_EXTICR3_EXTI8_0;
    EXTI->FTSR1 |= EXTI_FTSR1_FT8;              
    EXTI->IMR1 |= EXTI_IMR1_IM8;                 

    NVIC_SetPriority(EXTI4_15_IRQn, 0);
    NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void __attribute__((interrupt, used)) EXTI4_15_IRQHandler(void) {
    if (EXTI->FPR1 & EXTI_FPR1_FPIF8) {     
        EXTI->FPR1 |= EXTI_FPR1_FPIF8;      
        processClicked(EV_PLUS);
    } else if (EXTI->FPR1 & EXTI_FPR1_FPIF7) {     
        EXTI->FPR1 |= EXTI_FPR1_FPIF7;             
        processClicked(EV_MINUS);
    } else if (EXTI->FPR1 & EXTI_FPR1_FPIF6) {
        EXTI->FPR1 |= EXTI_FPR1_FPIF7;
        processClicked(EV_CONTROL);
    }
}