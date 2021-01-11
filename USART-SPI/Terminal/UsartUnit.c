#include "UsartUnit.h"

// USART2 -> AF07
// PA9  -> USART1_TX
// PA10 -> USART1_RX


void usart_init(void) {

	RCC->APB2ENR |= RCC_APB2ENR_USART1EN; // enable usart1 clock
	
	// TX 9
	GPIOA->MODER &= ~GPIO_MODER_MODE9_Msk; // reset
	GPIOA->MODER |= GPIO_MODER_MODE9_1; // pinA9 AF
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR9; //reset
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR9_1; // very fast speed
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH1;
	GPIOA->AFR[1] |= maskPos(0x7,(4*1)); // pinA9 AF to AF7
	
	// RX 10
	GPIOA->MODER &= ~GPIO_MODER_MODE10_Msk; // reset
	GPIOA->MODER |= GPIO_MODER_MODE10_1; // pinA10 AF
	GPIOA->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR10; //reset
	GPIOA->OSPEEDR |= GPIO_OSPEEDER_OSPEEDR10_1; // very fast speed
	GPIOA->AFR[1] &= ~GPIO_AFRH_AFRH2;
	GPIOA->AFR[1] |= maskPos(0x7,(4*2)); // pinA10 AF to AF7

	// interupt
	NVIC_SetPriority(USART1_IRQn, 0);
	NVIC_ClearPendingIRQ(USART1_IRQn);
	NVIC_EnableIRQ(USART1_IRQn);
	USART1->CR1 |= USART_CR1_RXNEIE; // enable interrupt for rxne
	
	USART1->CR1 |= USART_CR1_UE; // enable usart
	USART1->BRR = 0x0683; // set baud rate 16Mhz 9600baud
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmission enable, receive enable
}


void usart_write(uint16_t data) {
	USART1->DR = usart_encode( (data & ((uint16_t) 0x01FF)) );
	while(!READ_BIT(USART1->SR,USART_SR_TC)) {}
}

uint16_t usart_read(void) {
	while(!READ_BIT(USART1->SR, USART_SR_RXNE)) {}
	return usart_decode((uint16_t)(USART1->DR & (uint16_t)0x01FF) );
}

uint16_t usart_encode(uint16_t data) {
	return data + 'A';
}

uint16_t usart_decode(uint16_t data) {
	return data - 'A';
}

