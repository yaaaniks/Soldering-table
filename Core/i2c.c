#include "i2c.h"


/// @brief  I2C2 initialization for PA11->SCL, PA11->SDA, 100kHz. This function 
// 			configures and enables i2c bus with priority of 2.
void I2C2_Config(void) {
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN; 
	__NOP();
	__NOP();
	RCC->APBENR1 |= RCC_APBENR1_I2C2EN;
	__NOP();
	__NOP(); 
	GPIOA->MODER |= GPIO_MODER_MODE11_1 ; 
	GPIOA->MODER &= ~(GPIO_MODER_MODE11_0); 

	GPIOA->MODER |= GPIO_MODER_MODE12_1 ; 
	GPIOA->MODER &= ~(GPIO_MODER_MODE12_0); 

	GPIOA->OTYPER |= GPIO_OTYPER_OT11 ;
	GPIOA->OTYPER |= GPIO_OTYPER_OT12 ;

	GPIOA->PUPDR |= GPIO_PUPDR_PUPD11_0 ;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD11_1);

	GPIOA->PUPDR |= GPIO_PUPDR_PUPD12_0 ;
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD12_1);

	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED11_0 ;
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED11_1);
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED12_0 ;
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED12_1);

    GPIOA->AFR[1] |= (GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2 |
					  GPIO_AFRH_AFSEL12_1 | GPIO_AFRH_AFSEL12_2);    

    I2C2->CR1 &= ~I2C_CR1_PE;

	__NOP();
    I2C2->CR1 |= I2C_CR1_ANFOFF;
	I2C2->TIMINGR = 0x00303D5B;
	// I2C2->TIMINGR |= 0 << 28UL;
	// I2C2->CR1 |= I2C_CR1_TCIE; //Transfer Complete interrupt enable TCIE = 1
	// I2C2->CR1 |= I2C_CR1_TXIE; // Transmit (TXIS) interrupt enabled TXIE = 1
	// I2C2->CR1 |= I2C_CR1_RXIE; // Receive (RXNE) interrupt enabled RXIE = 1
	// I2C2->CR1 |= I2C_CR1_STOPIE; // STOP (STOPF) interrupt enabled STOPIE = 1
	I2C2->CR1 |= I2C_CR1_PE;
	__NOP();
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
	I2C2->CR2 |= (uint32_t)(size + 2 << 16); 
	I2C2->CR2 |= (1U << 25); 						
	I2C2->CR2 |= (1U << 13); 
	while(size) {						
		timeOut = timeout;
		while( !(I2C2->ISR & (I2C_ISR_TXIS))) {
			if(!timeOut)
				break;
		}			
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
		timeOut = timeout;
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

    timeOut = timeout;
	while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
        if(!timeOut)
            return;
    }

    I2C2->TXDR = (uint32_t)memAddr;
    timeOut = timeout;
	while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
        if(!timeOut)
            return;
    }					

	// FIXME: incorrect flag is read in status register 
	for (uint32_t index = 0; index < size; index++) {
		I2C2->TXDR = *(data++);
		timeOut = timeout;
        while(!(I2C2->ISR & (I2C_ISR_TXIS))) {
			if(!timeOut)
				break;
		}
    } 
}

/**
 * @brief 
 ** This function reads from i2c devices with 16bit address.
 ** it doesn't need delays.
 ** it divides address into 2 pieces; HIGH ADDr, LOW ADDr
 * 
 * @param devAddr slave device address
 * @param memAddr slave device address
 * @param data data pointer
 * @param size data size
 */
void I2C2_MemRead(const uint8_t devAddr, uint16_t memAddr, uint8_t* data, uint32_t size) {
	I2C2->CR2 = 0;
	I2C2->CR2 |= (uint32_t)(devAddr);
	I2C2->CR2 |= (2U << 16); 						
	I2C2->CR2 |= (1U << 13); 						

	while(!(I2C2->ISR & (I2C_ISR_TXIS)));
	I2C2->TXDR = (uint32_t)(memAddr >> 8);			

	while(!(I2C2->ISR & (I2C_ISR_TXIS)));
	I2C2->TXDR = (uint32_t)(memAddr & 0xFF);		

	while(!(I2C2->ISR & (I2C_ISR_TC)));					

	I2C2->CR2 =0;
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
