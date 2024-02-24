#include "adc.h"
#include "timer.h"

static volatile uint16_t raw[SMP_COUNT] __attribute__((aligned(2)));

void adcInit(void) {
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	__NOP(); __NOP();								  
	RCC->AHBRSTR |= RCC_AHBRSTR_DMA1RST;
	__NOP(); __NOP();								  
	RCC->AHBRSTR &= ~RCC_AHBRSTR_DMA1RST;
	__NOP(); __NOP();								  

	RCC->APBENR2 |= RCC_APBENR2_ADCEN;
	__NOP(); __NOP();								  
	RCC->APBRSTR2 |= RCC_APBRSTR2_ADCRST;
	__NOP(); __NOP();								
	RCC->APBRSTR2 &= ~RCC_APBRSTR2_ADCRST;
	__NOP(); __NOP();									

	ADC1->CFGR2 &= ~(ADC_CFGR2_CKMODE | ADC_CFGR2_OVSR | ADC_CFGR2_OVSS);
	//? TODO: add oversampling
	// ADC1->CFGR2 |= ADC_CFGR2_OVSE | (3 << ADC_CFGR2_OVSS_Pos) | (2 << ADC_CFGR2_OVSR_Pos);

	ADC1->CR |= ADC_CR_ADVREGEN;
	delay_ms(20);

	ADC->CCR &= ~ADC_CCR_PRESC;
	ADC->CCR |= ADC_CCR_PRESC_2; // SYSCLK = 64MHz --> ADC clk = 64 / 4 = 16 MHz         

	__NOP();								
	__NOP();								

	/* ADC calibration */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) {
		ADC1->CR &= (uint32_t)(ADC_CR_ADDIS);
	}

	ADC1->CR |= ADC_CR_ADCAL; 			

	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) {}
	ADC1->ISR |= ADC_ISR_EOCAL; 			

	for(uint8_t i = 0; i < 48; i++) __NOP();							

	ADC1->SMPR &= ~(ADC_SMPR_SMPSEL);
	// ADC clk = 16 MHz --> 16.000.000 / 160,5 + 1,5 ~ 100 KHz - max freq of TRGO 
	ADC1->SMPR |= (ADC_SMPR_SMPSEL5 | ADC_SMPR_SMP2_0 | ADC_SMPR_SMP2_1 | ADC_SMPR_SMP2_2);

	/* ADC configuration */
	ADC1->CFGR1 &= ~(ADC_CFGR1_ALIGN | ADC_CFGR1_RES);		

	ADC1->ISR &= ~ADC_ISR_CCRDY;         
	ADC1->CHSELR = ADC_CHSELR_CHSEL5;
	ADC1->CFGR1 &= ~(ADC_CFGR1_EXTSEL | ADC_CFGR1_CHSELRMOD | ADC_CFGR1_SCANDIR | ADC_CFGR1_CONT);
	__DMB();             
	ADC1->CFGR1 |= (ADC_CFGR1_AUTOFF | ADC_CFGR1_DISCEN | ADC_CFGR1_EXTEN | ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0); // TIM3 TRGO
	__DMB();
	while((ADC1->ISR & ADC_ISR_CCRDY) == 0) {}


	/* DMA CONFIGURATION */

	ADC1->CFGR1 |= (ADC_CFGR1_DMACFG | ADC_CFGR1_DMAEN); 

	DMA1->IFCR = (DMA_IFCR_CTCIF1 | DMA_IFCR_CGIF1);

	DMAMUX1_Channel0->CCR = 0;
	DMAMUX1_Channel0->CCR = 5 << DMAMUX_CxCR_DMAREQ_ID_Pos;
	
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
	DMA1_Channel1->CMAR = (uint32_t)&(raw);
	DMA1_Channel1->CNDTR = SMP_COUNT;
	DMA1_Channel1->CCR = (DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_TCIE);
	__DMB();

	ADC1->IER |= ADC_ISR_ADRDY;
	ADC1->CR |= ADC_CR_ADEN; 			

	// while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}

	delay_ms(20);	 

	NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);

	DMA1_Channel1->CCR |= DMA_CCR_EN;
	
	TIM3->CR1 |= TIM_CR1_CEN;
	ADC1->CR |= ADC_CR_ADSTART;
}

void __attribute__((interrupt, used)) DMA1_Channel1_IRQHandler() {
	if (DMA1->ISR & DMA_ISR_TCIF1) {
		DMA1->IFCR = DMA_IFCR_CTCIF1;
		updateTemp(raw);
	}
}
