#include "rcc.h"

void SystemClockInit_64MHz(void) {
	RCC->CR |= RCC_CR_HSION;			// Enable HSI16
	while(!(RCC->CR & RCC_CR_HSIRDY));	// Wait for cyrstal

	FLASH->ACR |= (FLASH_ACR_DBG_SWEN | FLASH_ACR_PRFTEN);		// CPU Prefetch enable
	FLASH->ACR &= ~FLASH_ACR_LATENCY_0;		// Flash memory access latency; reset
	FLASH->ACR &= ~(1U << 1);				// Flash memory access latency; reset
	FLASH->ACR &= ~(1U << 2);				// Flash memory access latency; reset
	FLASH->ACR |= FLASH_ACR_LATENCY_0;		// Flash memory access latency; set to 1 wait state

	RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_1;		// Select HSI16 for pll source
	RCC->PLLCFGR |= (0U << 4);					// PLLM division /1
	RCC->PLLCFGR &= ~(0x7FU << 8);	// PLL frequency multiplication factor N; reset
	RCC->PLLCFGR |= (0x8U << 8);	// PLL frequency multiplication factor N; x8
	RCC->PLLCFGR &= ~(1U << 29);	// PLL VCO division factor R for PLLRCLK clock output; reset
	RCC->PLLCFGR &= ~(1U << 30);	// PLL VCO division factor R for PLLRCLK clock output; reset
	RCC->PLLCFGR &= ~(1U << 31);	// PLL VCO division factor R for PLLRCLK clock output; reset
	RCC->PLLCFGR |= (1U << 29);		// PLL VCO division factor R for PLLRCLK clock output; /2
	RCC->PLLCFGR |= (1U << 28);		// PLLRCLK clock output enable

	RCC->CFGR &= ~(1U << 11);		// AHB prescaler to /1
	RCC->CFGR &= ~(1U << 14);		// APB prescaler to /1

	RCC->CR |= RCC_CR_PLLON;			// PLLON: PLL enable
	while(!(RCC->CR & RCC_CR_PLLRDY));	// Wait for PLL to stable

	RCC->CFGR &= ~RCC_CFGR_SW_0;		// System clock switch to; reset
	RCC->CFGR &= ~(1U << 1);			// System clock switch to; reset
	RCC->CFGR |= RCC_CFGR_SW_1;			// System clock switch to; PLLRCLK

	while(!(RCC->CFGR & RCC_CFGR_SWS_1));	// Wait for PLL to stable

	SystemCoreClockUpdate();	// Update system clock
}

void RCC_DeInit(void) {
    RCC->CR = RCC_CR_HSION;
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CR &= ~(RCC_CR_PLLON);
	SystemCoreClockUpdate();
}