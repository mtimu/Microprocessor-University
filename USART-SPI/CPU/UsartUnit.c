#include "UsartUnit.h"

// USART2 -> AF07
// PA0 -> USART2_CTS
// PA1 -> USART2_RTS
// PA2 -> USART2_TX
// PA3 -> USART2_RX
// PA4 -> USART2_CK


void usart_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN; // enable usart1 clock
	
	// TX
	GPIOA->MODER &= ~GPIO_MODER_MODE2; // reset
	GPIOA->MODER |= GPIO_MODER_MODE2_1; // pinA2 AF
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR2; //reset
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR2_1; // very fast speed
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL2;
	GPIOA->AFR[0] |= maskPos(0x7,(4*2)); // pinA2 AF to AF7
	
	// RX
	GPIOA->MODER &= ~GPIO_MODER_MODE3; // reset
	GPIOA->MODER |= GPIO_MODER_MODE3_1; // pinA3 AF
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR3; //reset
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR3_1; // very fast speed
	GPIOA->AFR[0] &= ~GPIO_AFRL_AFRL3;
	GPIOA->AFR[0] |= maskPos(0x7,(4*3)); // pinA3 AF to AF7
	


	// interupt
	NVIC_SetPriority(USART2_IRQn, 0);
	NVIC_ClearPendingIRQ(USART2_IRQn);
	NVIC_EnableIRQ(USART2_IRQn);
	USART2->CR1 |= USART_CR1_RXNEIE; // enable interrupt for rxne
	
	USART2->CR1 |= USART_CR1_UE; // enable usart
	USART2->BRR = 0x0683; // set baud rate 16Mhz 9600baud
	USART2->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmission enable, receive enable
}


void usart_write(uint16_t data) {
	USART2->DR = usart_encode((data & ((uint16_t) 0x01FF)) );
	while(!READ_BIT(USART2->SR,USART_SR_TC)) {}
}

uint16_t usart_read(void) {
	while(!READ_BIT(USART2->SR, USART_SR_RXNE)) {}
	return usart_decode((uint16_t)(USART2->DR & (uint16_t)0x01FF));
}

uint16_t usart_encode(uint16_t data) {
	return data + 'A';
}

uint16_t usart_decode(uint16_t data) {
	return data - 'A';
}


