#include "adc.h"

/* Raw values from the ADC1 data register, copyied to this array by DMA channel 1 */
volatile uint16_t ADCRawValues[ADC_SRC_CNT] __attribute__((aligned(4)));

/* number of ADC1 end of sequence events since startup */
uint32_t ADCEOSCnt = 0;

/* systick value of the last start of the ADC (used for ADCDuration) [systicks] */
uint32_t ADCStartTime = 0;

/* value of ADC1->ISR inside the ADC interrupt after EOS, seems to be %0000000000001010 (independent from AUTOFF flag) */
uint32_t ADCISRAfterEOS = 0;

int32_t low_pass(int32_t *a, int32_t x, int32_t p) {
    int32_t n;
    // n = ((1<<LOW_PASS_BITS)-p) * (*a) + p * x + (1<<(LOW_PASS_BITS-1));
    n = ((1 << LOW_PASS_BITS) - p) * (*a) + p * x;
    n >>= LOW_PASS_BITS;
    *a = n;
    return n;
}

void ADC1_Init(void) {
    short i;
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

    /* ADC Reset */
    RCC->APBRSTR2 |= RCC_APBRSTR2_ADCRST;
    __NOP(); 
    __NOP(); 
    RCC->APBRSTR2 &= ~RCC_APBRSTR2_ADCRST;
    __NOP(); 
    __NOP(); 

    /* Allow ADC interrupts */

    /* ADC Basic Setup */

    ADC1->IER = 0;                    /* do not allow any interrupts */
    ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE; /* select HSI16 clock */

    /* oversampler */
    ADC1->CFGR2 &= ~ADC_CFGR2_OVSS;
    ADC1->CFGR2 &= ~ADC_CFGR2_OVSR;

    ADC1->CFGR2 |= ADC_CFGR2_OVSE | (3 << ADC_CFGR2_OVSS_Pos) // bit shift
                   | (2 << ADC_CFGR2_OVSR_Pos);               // 1: 4x, 2: 8x, 3: 16x

    ADC1->CR |= ADC_CR_ADVREGEN; /* enable ADC voltage regulator, 20us wait time required */
    ADC1->CR |= ADC_CFGR1_DMAEN;
    delay_ms(20);

    /* ADC Clock prescaler */
    ADC->CCR &= ~ADC_CCR_PRESC;
    ADC->CCR |= ADC_CCR_PRESC_2; /* divide by 0100=8 */

    ADC->CCR |= ADC_CCR_VREFEN;
    ADC->CCR |= ADC_CCR_TSEN;   
    // ADC->CCR |= ADC_CCR_VBATEN; 

    __NOP(); 
    __NOP(); 

    if ((ADC1->CR & ADC_CR_ADEN) != 0) {
        ADC1->CR &= (uint32_t)(ADC_CR_ADDIS);
    }

    ADC1->CR |= ADC_CR_ADCAL;
    // FIXME: loop
    // while ((ADC1->ISR & ADC_ISR_EOCAL) == 0) {}
    ADC1->ISR |= ADC_ISR_EOCAL;

    for (i = 0; i < 48; i++) 
        __NOP();             

    ADC1->SMPR &= ~ADC_SMPR_SMP1;
    ADC1->SMPR &= ~ADC_SMPR_SMP2;
    ADC1->SMPR |= ADC_SMPR_SMP1_2;
    ADC1->SMPR |= ADC_SMPR_SMP2_2;

    ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN; 
    ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN; 
    ADC1->CFGR1 &= ~ADC_CFGR1_RES;   

    ADC1->ISR &= ~ADC_ISR_CCRDY;

    ADC1->CFGR1 &= ~ADC_CFGR1_CHSELRMOD;
    // ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR;   
    // ADC1->CFGR1 &= ~ADC_CFGR1_DISCEN;  
    ADC1->CFGR1 |= ADC_CFGR1_CONT;       
    // ADC1->CFGR1 &= ~ADC_CFGR1_CONT;  
    ADC1->CFGR1 |= ADC_CFGR1_AUTOFF;
    ADC1->CFGR1 |= ADC_CFGR1_SCANDIR;  
    ADC1->CFGR1 |= ADC_CFGR1_EXTSEL_2; 
    ADC1->CHSELR = ADC_CHSELR_CHSEL0;
    while ((ADC1->ISR & ADC_ISR_CCRDY) == 0) {}

    ADC1->CFGR1 |= ADC_CFGR1_DMACFG; 
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN;  

    DMA1_Channel1->CCR = 0;
    DMA1_Channel1->CPAR = (uint32_t) & (ADC1->DR);
    DMA1_Channel1->CMAR = (uint32_t) & (ADCRawValues[0]);
    DMA1_Channel1->CNDTR = ADC_SRC_CNT;
    DMA1_Channel1->CCR = (DMA_CCR_PL | DMA_CCR_MSIZE_0 | DMA_CCR_PSIZE_0 | 
                          DMA_CCR_MINC | DMA_CCR_CIRC);    
    DMA1_Channel1->CCR |= DMA_CCR_TCIE | DMA_CCR_HTIE;



    DMA1_Channel1->CCR |= DMA_CCR_EN;

    ADC1->IER |= ADC_IER_EOSIE;
    ADC1->ISR |= ADC_ISR_ADRDY; 
    ADC1->CR |= ADC_CR_ADEN;    

    // FIXME: loop
    /* according to the datasheet, the ADRDY flag is not set if the AUTOFF flag is active. */
    /* Instead power on sequence is done automatically. */
    // while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) /* wait for ADC */
    // {}

    delay_ms(20);
    // ADC1->CR |= ADC_CR_ADSTART;
}

void ADC1_Start(void) {
    ADCStartTime = SysTick->VAL;
                                           
    DMAMUX1_Channel0->CCR = 0;
    DMAMUX1_Channel0->CCR = 5 << DMAMUX_CxCR_DMAREQ_ID_Pos; /* 5=ADC */

    DMA1_Channel1->CCR |= DMA_CCR_EN; 
    ADC1->CR |= ADC_CR_ADSTART;      
    NVIC_EnableIRQ(ADC1_IRQn); 
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);
    NVIC_SetPriority(DMA1_Channel1_IRQn, 2);
}

void __attribute__((interrupt, used)) ADC1_IRQHandler(void) {
    if ((ADC1->ISR & ADC_ISR_EOS) != 0) {
        ADCISRAfterEOS = ADC1->ISR;
        ADC1->ISR |= ADC_ISR_EOS;
        ADCEOSCnt++;
        updateTemp(ADCRawValues);
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
