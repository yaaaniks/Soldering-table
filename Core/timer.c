#include "timer.h"
#include "tempControl.h"
#include "constants.h"

volatile uint32_t SysTick_CNT = 0;
volatile uint32_t timeOut = 0;
uint8_t PWM_Fill = 20;

void SysTick_Init(void) {
    SystemCoreClockUpdate();                         
    SysTick_Config(SystemCoreClock/1000-1); // set tick to every 1ms	
}

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

//    /* массив значений ШИМ */
//    uint16_t pwm_array[20] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100,
//          90, 80, 70, 60, 50, 40, 30, 20, 10};

//    /* Генерация сигнала ШИМ, TIM14, Ch 1 (PA4), 100 Гц */
//    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
//    RCC->APB1ENR |= RCC_APB1ENR_TIM14EN;

//    GPIOA->AFR[0] |= 4 << (4*4);
//    GPIOA->MODER |= GPIO_MODER_MODER4_1;

//    TIM14->PSC = 800-1;
//    TIM14->ARR = 100-1;
//    TIM14->CCR1 = 0;
//    TIM14->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE; // PWM, Ch1
//    TIM14->CCER = TIM_CCER_CC1E;

//    /* Настройка DMA, Channel5, активация от TIM1 UPdate */
//    RCC->AHBENR |= RCC_AHBENR_DMAEN;

//    DMA1_Channel5->CCR = DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | // размерность 16/16 бит
//          DMA_CCR_MINC | // инкремент адресов памяти
//          DMA_CCR_DIR | // направление передачи mem->periph
//          DMA_CCR_CIRC; // циклический режим
//    DMA1_Channel5->CMAR = (uint32_t)pwm_array; // адрес источника
//    DMA1_Channel5->CPAR = (uint32_t)&TIM14->CCR1; // адрес назначения
//    DMA1_Channel5->CNDTR = 20; // число значений в одном цикле


//    /* Таймер интервалов изменения ШИМ, 500 мс интервал */
//    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;

//    TIM1->PSC = 8000-1;
//    TIM1->ARR = 500-1;
//    TIM1->DIER = TIM_DIER_UDE; // событие обновления счета активирует DMA

//    /* запуск */
//    DMA1_Channel5->CCR |= DMA_CCR_EN; // DMA канал 5 разрешен
//    TIM14->CR1 = TIM_CR1_CEN;   // таймер ШИМ запущен
//    TIM1->CR1 = TIM_CR1_CEN;    // таймер интервалов запущен
void TIM2_Init(void) {
    GPIOA->MODER &= ~(GPIO_MODER_MODE1_0);  ///
    GPIOA->MODER |= GPIO_MODER_MODE1_1; 
    GPIOA->AFR[0] |= 2U << 4;
	// GPIOA->OTYPER &= ~(GPIO_OTYPER_OT1);

	// GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD1);

	// GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED1;
	// GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED1_1);

    GPIOA->AFR[0] |= (GPIO_AFRL_AFSEL1_1 | GPIO_AFRL_AFSEL1_2); 
	// 				  GPIO_AFRH_AFSEL12_1 | GPIO_AFRH_AFSEL12_2);  
    GPIOA->AFR[0] |= (1U << 4 *1);	// AFSEL7 to AF1 0001
/*
* Инициализация TIM2->CH1 на генерацию PWM
*/
RCC->APBENR1 |= RCC_APBENR1_TIM2EN;                                // Включаем тактирование таймера
TIM2->PSC     |= 6399;                                              // При тактовой частоте 72 МГц, тактирование таймера 10КГц
TIM2->ARR     |= 100;                                               // Период ШИМ. Тактовая частота ШИМ будет равняться 10 000/100=100Гц
TIM2->CCR2    |= 20;                                                // Коэффициент заполнения ШИМ 20% (При таких значениях делителей).
TIM2->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2PE; // PWM, Ch1                     // Включаем PWM первого типа
TIM2->CCER    |= TIM_CCER_CC2E;                                     // Разрешаем первому каналу плеваться на выход
TIM2->CR1     |= TIM_CR1_CEN;                                       // Включаем таймер                                              
}

void __attribute__ ((interrupt, used)) TIM3_IRQHandler() {
	TIM3->SR = 0;	// clear pending bit in the interrupt handler
}

void __attribute__ ((interrupt, used)) TIM2_IRQHandler(void) {
	TIM2->SR = 0;	// clear pending bit in the interrupt handler
}

void __attribute__ ((interrupt, used)) TIM1_BRK_UP_TRG_COM_IRQHandler(void) {
    // TIM1->SR &= ~ TIM_SR_UIF;
    updateScreen();
    TIM1->SR = 0;
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