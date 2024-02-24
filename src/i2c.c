#include "i2c.h"

/**
 * @brief I2C2 initialization for 400 kHz, PA11 --> SCL, PA11 --> SDA.
 * 
 */
void i2cInit(void) {
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN; 
	__NOP(); __NOP();
	PWR->PUCRA |= PWR_PUCRA_PU11 | PWR_PUCRA_PU12;
	
	RCC->APBRSTR1 |= RCC_APBRSTR1_I2C2RST;
	__DMB();
	
	RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2C2RST;
	__DMB();
	
	RCC->APBENR1 |= RCC_APBENR1_I2C2EN;
	__NOP(); __NOP(); 

	GPIOA->MODER &= ~(GPIO_MODER_MODE11 | GPIO_MODER_MODE12);  
	GPIOA->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1);

	GPIOA->OTYPER |= (GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12);

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD11 | GPIO_PUPDR_PUPD12);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD12_0);

	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED11 | GPIO_OSPEEDR_OSPEED12);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED11_1 | GPIO_OSPEEDR_OSPEED12_1);

    GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2 |
					  GPIO_AFRH_AFSEL12_1 | GPIO_AFRH_AFSEL12_2);    
				
    I2C2->CR1 &= ~I2C_CR1_PE;

	I2C2->CR1 |= (I2C_CR1_ANFOFF | I2C_CR1_ERRIE | I2C_CR1_TXDMAEN | I2C_CR1_DNF);
	__DMB();
	I2C2->TIMINGR = (3 << I2C_TIMINGR_PRESC_Pos | 9 << I2C_TIMINGR_SCLL_Pos | 7 << I2C_TIMINGR_SCLH_Pos | 
				     2 << I2C_TIMINGR_SDADEL_Pos | 3 << I2C_TIMINGR_SCLDEL_Pos);
	__DMB();
	I2C2->CR1 |= I2C_CR1_PE;
	__ISB();
}

void i2cTransmit(const uint8_t devAddr, const uint8_t memAddr, const uint8_t* data, size_t size) {
	I2C2->CR2 = 0;									
	I2C2->CR2 &= ~(I2C_CR2_RD_WRN);
	I2C2->CR2 |= (uint32_t)((size + sizeof(devAddr)) << I2C_CR2_NBYTES_Pos | 
							 devAddr << I2C_CR2_SADD_Pos | I2C_CR2_AUTOEND);
	__DMB();
	I2C2->CR2 |= I2C_CR2_START; 				

	uint32_t timestamp = getTimestamp();
	while(!(I2C2->ISR & (I2C_ISR_TXE))) {
		if (getTimestamp() - timestamp > 1000) {
			return;
		}
	}

    I2C2->TXDR = (uint32_t)memAddr;
	while(!(I2C2->ISR & (I2C_ISR_TXE))) {
		if (getTimestamp() - timestamp > 1000) {
			return;
		}
	}					
 
	for (size_t index = 0; index < size; index++) {
		I2C2->TXDR = (uint32_t)*(data++);
        while(!(I2C2->ISR & I2C_ISR_TXE)) {
			if (getTimestamp() - timestamp > 1000) {
				return;
			}
		}
    } 
}
