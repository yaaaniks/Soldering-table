#include "adc.h"
#include "timer.h"

/* Raw values from the ADC1 data register, copyied to this array by DMA channel 1 */
volatile uint16_t ADCRawValues[ADC_SRC_CNT] __attribute__((aligned(4)));

/* number of ADC1 end of sequence events since startup */
uint32_t ADCEOSCnt = 0;

/* systick value of the last start of the ADC (used for ADCDuration) [systicks] */
uint32_t ADCStartTime = 0;

/* value of ADC1->ISR inside the ADC interrupt after EOS, seems to be %0000000000001010 (independent from AUTOFF flag) */
uint32_t ADCISRAfterEOS = 0;



void adcInit(void) {
	short i;
	PWR->PUCRA |= PWR_PUCRA_PU5;
	PWR->PDCRA |= PWR_PDCRA_PD5; 

	GPIOA->PUPDR |= GPIO_PUPDR_PUPD5_1;
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;
	__NOP();								
	__NOP();								  
	RCC->AHBRSTR |= RCC_AHBRSTR_DMA1RST;
	__NOP();								
	__NOP();								  
	RCC->AHBRSTR &= ~RCC_AHBRSTR_DMA1RST;
	__NOP();								
	__NOP();								  

	RCC->APBENR2 |= RCC_APBENR2_ADCEN;
	__NOP();								
	__NOP();								  
	RCC->APBRSTR2 |= RCC_APBRSTR2_ADCRST;
	__NOP();								
	__NOP();								
	RCC->APBRSTR2 &= ~RCC_APBRSTR2_ADCRST;
	__NOP();								
	__NOP();								

	NVIC_EnableIRQ(ADC1_IRQn);

	ADC1->IER = 0;					
	ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;	
	ADC1->CFGR2 &= ~ADC_CFGR2_OVSS;
	ADC1->CFGR2 &= ~ADC_CFGR2_OVSR;

	ADC1->CFGR2 |= ADC_CFGR2_OVSE
		| (3 << ADC_CFGR2_OVSS_Pos)         // bit shift
		| (2 << ADC_CFGR2_OVSR_Pos);         // 1: 4x, 2: 8x, 3: 16x

	ADC1->CR |= ADC_CR_ADVREGEN;
	delay_ms(20);

	ADC->CCR &= ~ADC_CCR_PRESC;
	ADC->CCR |= ADC_CCR_PRESC_2;             

	// ADC1->IER = 0;				
	// ADC1->IER |= ADC_IER_EOCALIE;		
	// ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;	// clear the clock mode, which would the select the async clock source from RCC, which is the 64MHz system clock by default
	// ADC1->CFGR2 |= ADC_CFGR2_CKMODE_0; // select PCLK/2 --> 32MHz ADC clock

	// ADC1->CFGR2 &= ~ADC_CFGR2_OVSE; // disable oversampler
	// ADC1->CFGR2 &= ~ADC_CFGR2_OVSS;  // clear oversampling shift
	// ADC1->CFGR2 &= ~ADC_CFGR2_OVSR;  // clear oversampling ratio


	// ADC->CCR |= ADC_CCR_VREFEN; 			/* Wake-up the VREFINT */  
	// ADC->CCR |= ADC_CCR_TSEN; 			/* Wake-up the temperature sensor */  
	//ADC->CCR |= ADC_CCR_VBATEN; 			/* Wake-up VBAT sensor */  

	__NOP();								
	__NOP();								

	/* CALIBRATION */
	if ((ADC1->CR & ADC_CR_ADEN) != 0) /* clear ADEN flag if required */
	{
		ADC1->CR &= (uint32_t)(ADC_CR_ADDIS);
	}

	ADC1->CR |= ADC_CR_ADCAL; 				/* start calibration */

	while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) 	/* wait for clibration finished */
	{
	}
	ADC1->ISR |= ADC_ISR_EOCAL; 			/* clear the status flag, by writing 1 to it */

	for( i = 0; i < 48; i++ )                             /* post calibration delay */
		__NOP();								/* not sure why, but some nop's are required here, at least 8 of them with 16MHz */

	/* CONFIGURE ADC */
	/*
	ADC1->SMPR &= ~ADC_SMPR_SMP1;
	ADC1->SMPR &= ~ADC_SMPR_SMP2;
	ADC1->SMPR |= ADC_SMPR_SMP1_2; 
	ADC1->SMPR |= ADC_SMPR_SMP2_2; 

	ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;	
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;	
	ADC1->CFGR1 &= ~ADC_CFGR1_RES;		
	*/

	ADC1->SMPR &= ~ADC_SMPR_SMP1;
	ADC1->SMPR &= ~ADC_SMPR_SMP2;

	/* ADC result configuration */
	ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;		/* right alignment */
	ADC1->CFGR1 &= ~ADC_CFGR1_RES;		/* 12 bit resolution */
	/* CONFIGURE SEQUENCER */

	ADC1->ISR &= ~ADC_ISR_CCRDY;          /* clear the channel config flag */

	/*
	ch 12: temperture sensor
	ch 13: vrefint
	ch 14: vbat
	*/
	ADC1->CFGR1 &= ~ADC_CFGR1_CHSELRMOD;  /* "not fully configurable" mode */
	ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR;    /* forward scan */
	ADC1->CFGR1 &= ~ADC_CFGR1_DISCEN;     /* disable discontinues mode */
	//ADC1->CFGR1 |= ADC_CFGR1_CONT;        /* continues mode */
	ADC1->CFGR1 &= ~ADC_CFGR1_CONT;        /* disable continues mode: excute the sequence only once  */
	ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;     /* enable auto off feature, according to the datasheet, the ADRDY flag will not be raised if this feature is enabled */

	ADC1->CFGR1 &= ~(ADC_CFGR1_EXTSEL);
	ADC1->CFGR1 |= ADC_CFGR1_EXTSEL_1 | ADC_CFGR1_EXTSEL_0; // TIM3 TRGO
	ADC1->CFGR1 |= ADC_CFGR1_EXTEN;

	ADC1->CHSELR = ADC_CHSELR_CHSEL5;          /* external temperature sensor */
		// ADC_CHSELR_CHSEL12 |                /* internal temperature sensor */
		// ADC_CHSELR_CHSEL13 ;                /* internal reference voltage (bandgap) */

	while((ADC1->ISR&ADC_ISR_CCRDY) == 0) /* wait until channel config is applied */
	{
	}


	/* DMA CONFIGURATION */

	ADC1->CFGR1 |= (ADC_CFGR1_DMACFG | ADC_CFGR1_DMAEN); 

	/* DMA CHANNEL SETUP */

	DMAMUX1_Channel0->CCR = 0;
	DMAMUX1_Channel0->CCR = 5 << DMAMUX_CxCR_DMAREQ_ID_Pos;
	DMA1_Channel1->CCR = 0;
	DMA1_Channel1->CPAR = (uint32_t)&(ADC1->DR);
	DMA1_Channel1->CMAR = (uint32_t)&(ADCRawValues[0]);
	DMA1_Channel1->CNDTR = ADC_SRC_CNT;
	DMA1_Channel1->CCR = (DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | 
						  DMA_CCR_MINC | DMA_CCR_CIRC | DMA_CCR_TCIE);


	/* ENABLE ADC */

	ADC1->IER |= (ADC_IER_EOSIE | ADC_ISR_ADRDY);
	ADC1->CR |= ADC_CR_ADEN; 			

	/* according to the datasheet, the ADRDY flag is not set if the AUTOFF flag is active. */
	/* Instead power on sequence is done automatically. */ 
	//while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* wait for ADC */
	//{
	//}

	delay_ms(20);	

	NVIC_SetPriority(ADC1_IRQn, 2);
	NVIC_EnableIRQ(ADC1_IRQn); 
	NVIC_SetPriority(DMA1_Channel1_IRQn, 1);
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	DMA1_Channel1->CCR |= DMA_CCR_EN;

	ADC1->CR |= ADC_CR_ADSTART;
}

void __attribute__((interrupt, used)) ADC1_IRQHandler(void) {
	if ((ADC1->ISR & ADC_ISR_EOS) != 0) {
		ADCISRAfterEOS = ADC1->ISR;
		ADC1->ISR |= ADC_ISR_EOS;
		ADCEOSCnt++;
	}
}

// FIXME: handler is not called
void __attribute__((interrupt, used)) DMA1_Channel1_IRQHandler() {
	if (DMA1->ISR & DMA_ISR_TCIF1) {
		DMA1->IFCR = DMA_ISR_TCIF1;
		updateTemp(ADCRawValues);
	}
	if (DMA1->ISR & DMA_ISR_HTIF1) {
		DMA1->IFCR = DMA_ISR_HTIF1;
		updateTemp(ADCRawValues);
	}
}
