#include "stm32f4xx.h"                  // Device header
#include "MaxUnit.h"
#include "Utils.h"




void max_init(void){
	max_spi_init();
	//max_write(MAX_DECODING_MODE,0xFF);

	max_write(MAX_SCAN_LIMIT,3);
	max_write(MAX_INTENSITY_LIGHT,15);
	max_write(MAX_TEST_DISPLAY,0);
	max_write(MAX_TURN_ON,1);
	
	
}

void max_spi_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable gpioa clock
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // enable spi clock
	
	// SPI mapping : AF05
	// SPI1_NSS  -> PA4
	// SPI1_SCLK -> PA5
	// SPI1_MOSI -> PA7
	
	GPIOA->MODER &= ~GPIO_MODER_MODE5_Msk; // reset pinA5 mode
	GPIOA->MODER |= GPIO_MODER_MODE5_1; // set pinA5 as alternative function
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL5; // reset alt func.
	GPIOA->AFR[0] |= maskPos(5,(5*4)); // set alt func 5
	
	GPIOA->MODER &= ~GPIO_MODER_MODE7_Msk; // reset pinA7 mode
	GPIOA->MODER |= GPIO_MODER_MODE7_1; // set pinA7 as alternative function
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL7; // reset alt func.
	GPIOA->AFR[0] |= maskPos(5,(7*4)); // set alt func 5
	
	GPIOA->MODER &= ~GPIO_MODER_MODE4; // reset pinA4
	GPIOA->MODER |= GPIO_MODER_MODE4_0; // set as output
	GPIOA->ODR |= mask(4);

	
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI | (4 << 3); // set as master enable ssm and ssi for software slave select
	SPI1->CR2 = 0; // disable all interrupts and dma
	SPI1->CR1 |= SPI_CR1_SPE; // enable SPI1
	
}

void max_write(uint8_t command, uint8_t data){
	while(!(SPI1->SR & SPI_SR_TXE)){} // wait until transmit buffer becomes empty
	GPIOA->ODR &= ~mask(4); // deassert slave select
	SPI1->DR = command;
	while(!(SPI1->SR & SPI_SR_TXE)){}
	SPI1->DR = data;
	while((SPI1->SR & SPI_SR_BSY)){} // wait for transmission completion
	GPIOA->ODR |= mask(4); // assert slave select
}

void max_reset(void) {
	// reset max_data and write 0 to max
	uint8_t i;
	for(i = 0; i < 4; i++){
		max_data[i] = 0;
	}
	
	max_loadData();
	
}

void max_err(void) {
	max_data[3] = 0;
	max_data[2] = MAX_E;
	max_data[1] = MAX_R;
	max_data[0] = MAX_R;
	max_loadData();
}

void max_putRight(uint8_t data) {
	// shift data
	uint8_t i;
	for (i = 3; i > 0; i--) {
		max_data[i] = max_data[i-1];
	}
	max_data[0] = data;
	
	max_loadData();
}

	// write whole data to max
void max_loadData(void) {
	uint8_t i;
	for(i = 0; i < 4; i++){
		max_write(MAX_DIGIT0+i,max_decode(max_data[i]));
	}
}

uint8_t max_decode(uint8_t data) {
	uint8_t result;
	switch(data) {
		case 1: 		result = 0x30; break;
		case 2: 		result = 0x6D; break;
		case 3: 		result = 0x79; break;
		case 4: 		result = 0x33; break;
		case 5: 		result = 0x5B; break;
		case 6: 		result = 0x5F; break;
		case 7: 		result = 0x70; break;
		case 8: 		result = 0x7F; break;
		case 9: 		result = 0x7B; break;
		case MAX_E: result = 0x4F; break;
		case MAX_R: result = 0x46; break;
		default:
		case 0: result = 0x7E;
	}
	return result;
}




