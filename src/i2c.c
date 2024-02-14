#include "i2c.h"

// TODO add DMAMUX configuration
static void dmaInit(void) {
	RCC->AHBRSTR |= RCC_AHBRSTR_DMA1RST;
	__DMB();
	RCC->AHBRSTR &= ~RCC_AHBRSTR_DMA1RST;

	RCC->AHBENR |= RCC_AHBENR_DMA1EN;

}

static void dmaSet(const void *pData, size_t size) {

}

/**
 * @brief I2C2 initialization for 100 kHz, PA11 --> SCL, PA11 --> SDA.
 * 
 */
void i2cInit(void) {
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN; 
	__NOP(); __NOP();
	
	RCC->APBENR1 |= RCC_APBENR1_I2C2EN;
	__NOP(); __NOP(); 
	
	RCC->APBRSTR1 |= RCC_APBRSTR1_I2C1RST;
	__DMB();
	
	RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2C1RST;
	__DMB();

	GPIOA->MODER &= ~(GPIO_MODER_MODE11_0 | GPIO_MODER_MODE12_0);  
	GPIOA->MODER |= (GPIO_MODER_MODE11_1 | GPIO_MODER_MODE12_1);

	GPIOA->OTYPER |= (GPIO_OTYPER_OT11 | GPIO_OTYPER_OT12);

	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD11_1 | GPIO_PUPDR_PUPD12_1);
	GPIOA->PUPDR |= (GPIO_PUPDR_PUPD11_0 | GPIO_PUPDR_PUPD12_0);

	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED11_1 | GPIO_OSPEEDR_OSPEED12_1);
	GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEED11_0 | GPIO_OSPEEDR_OSPEED12_0);

    GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2 |
					  GPIO_AFRH_AFSEL12_1 | GPIO_AFRH_AFSEL12_2);    
				
    I2C2->CR1 &= ~I2C_CR1_PE;

	I2C2->CR1 |= (I2C_CR1_ANFOFF | I2C_CR1_ERRIE | I2C_CR1_DNF | I2C_CR1_TXDMAEN);
	__DMB();
	I2C2->TIMINGR |= (1 << I2C_TIMINGR_PRESC_Pos | 9 << I2C_TIMINGR_SCLL_Pos | 3 << I2C_TIMINGR_SCLL_Pos | 
					  2 << I2C_TIMINGR_SDADEL_Pos | 3 << I2C_TIMINGR_SCLDEL_Pos);
	__DMB();
	I2C2->CR1 |= I2C_CR1_PE;
	__ISB();
}

void i2cTransmit(const uint8_t devAddr, const uint8_t* data, size_t size) {
	I2C2->CR2 = 0;
	I2C2->CR2 |= (I2C_CR2_AUTOEND | size << I2C_CR2_NBYTES_Pos | I2C_CR2_NBYTES_Pos |
				  devAddr << I2C_CR2_SADD_Pos );
	I2C2->CR2 |= I2C_CR2_START;
}

/**
 * @brief 
 ** This function writes data to i2c devices without address.
 **	it writes data faster than other functions. ('with address' functions)
 **	it writes data wherever address pointer points at.
 * 
 * @param devAddr slave device address
 * @param data data pointer
 * @param size data size in bytes
 */
void I2C2_Write(const uint8_t devAddr, const uint8_t* data, uint32_t size, uint32_t timeout) {
	I2C2->CR2 =0;									
	I2C2->CR2 |= (uint32_t)(devAddr << 1);			
	I2C2->CR2 |= (uint32_t)(size << 16); 
	I2C2->CR2 |= (1U << 25); 						
	I2C2->CR2 |= (1U << 13); 
	while(size) {
		while( !(I2C2->ISR & (I2C_ISR_TXIS))) {}			
		I2C2->TXDR = (*data++);						
		size--;										
	}
}

/**
 * @brief 
 ** This function reads from i2c devices without address.
 ** it reads data faster than other functions. ('with address' functions)
 **	it reads data wherever address pointer points at.
 * 
 * @param devAddr 
 * @param data 
 * @param size 
 */
void I2C2_Read(const uint8_t devAddr, uint8_t* data, uint32_t size, uint32_t timeout) {
	I2C2->CR2 = 0;
	I2C2->CR2 |= (uint32_t)(devAddr);
	I2C2->CR2 |= (1U << 10); 					
	I2C2->CR2 |= (uint32_t)(size << 16); 		
	I2C2->CR2 |= (1U << 25); 					
	I2C2->CR2 |= (1U << 13); 					

	while(size) {
		while( !(I2C2->ISR & (I2C_ISR_RXNE)));
		(*data++) = (uint8_t)I2C2->RXDR;
		size--;
	}
}

/**
 * @brief 
 ** This function writes data to i2c devices with 16bit address.
 ** it needs amount of delay for device specific/
 * @param devAddr slave device address
 * @param memAddr slave memory address
 * @param data data pointer
 * @param size data size
 * @param timeout timeout to end transfers
 */
void I2C2_MemWrite(const uint8_t devAddr, const uint16_t memAddr, const uint8_t* data, uint32_t size, uint32_t timeout) {
	I2C2->CR2 = 0;									
	I2C2->CR2 |= (uint32_t)(devAddr);
	I2C2->CR2 |= (uint32_t)((size + 1) << 16); 		
	I2C2->CR2 &= ~(I2C_CR2_RD_WRN); 		
	I2C2->CR2 |= I2C_CR2_AUTOEND; 				 
	I2C2->CR2 |= I2C_CR2_START; 				

	while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
    }

    I2C2->TXDR = (uint32_t)memAddr;
	while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
    }					

	// FIXME: incorrect flag is read in status register 
	for (uint32_t index = 0; index < size; index++) {
		I2C2->TXDR = *(data++);
        while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
		}
    } 
}
