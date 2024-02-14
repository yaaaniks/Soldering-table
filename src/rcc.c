#include "rcc.h"

void SystemClockInit_64MHz(void) {
	RCC->CR |= RCC_CR_HSION;			
	while(!(RCC->CR & RCC_CR_HSIRDY));	

	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= (FLASH_ACR_LATENCY_0 | FLASH_ACR_DBG_SWEN | FLASH_ACR_PRFTEN);

	PWR->CR3 |= PWR_CR3_APC;

	/* • fVCO = fPLLIN × (N / M) --> fPLLP = fVCO / P --> fPLLQ = fVCO / Q --> fPLLR = fVCO / R */
	RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLSRC | RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN |
					  RCC_PLLCFGR_PLLP | RCC_PLLCFGR_PLLQ | RCC_PLLCFGR_PLLR);
	__DMB();
	RCC->PLLCFGR |= (RCC_PLLCFGR_PLLSRC_1 | RCC_PLLCFGR_PLLM_0 | RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLREN);

	RCC->CFGR &= ~(RCC_CFGR_SW | RCC_CFGR_HPRE | RCC_CFGR_PPRE);
	RCC->CFGR |= (RCC_CFGR_PPRE_0 | RCC_CFGR_PPRE_2);  
	__DMB();

	RCC->CR |= RCC_CR_PLLON;			
	while(!(RCC->CR & RCC_CR_PLLRDY)) {};	

	RCC->CFGR &= ~RCC_CFGR_SW;		
	RCC->CFGR |= RCC_CFGR_SW_1;

	while(!(RCC->CFGR & RCC_CFGR_SWS_PLLRCLK)) {};
	
	SystemCoreClockUpdate();
	__ISB();
	SysTick_Config(SystemCoreClock / 1000);
}

void RCC_DeInit(void) {
    RCC->CR = RCC_CR_HSION;
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CR &= ~(RCC_CR_PLLON);
	SystemCoreClockUpdate();
}