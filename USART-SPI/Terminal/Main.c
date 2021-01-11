#include "stm32f4xx.h"                  // Device header
#include "Main.h"
#include "stdint.h"
#include "MaxUnit.h"
#include "KeypadUnit.h"
#include "Utils.h"
#include "UsartUnit.h"


int main(void) {
	__disable_irq();
	keypad_init();
	max_init();
	usart_init();
	__enable_irq();
	

	while(1) {
		uint16_t key = getPressedKey();
		usart_write(key);
		delay(50);
	}
}


void USART1_IRQHandler(void) {
	NVIC_ClearPendingIRQ(USART1_IRQn);
	
	if(READ_BIT(USART1->SR, USART_SR_RXNE)) { // read enable
		uint16_t data = usart_read();
		cpu_data_handler(data);
	}
}


void cpu_data_handler(uint16_t data) {
	switch(data){
		case USART_STAR:
			max_reset();
		break;
		case USART_SIGN_NUMBER:
			// do nothing
		break;
		default: // numbers
			max_putRight( 0xFF & data);
	}
}

void delay(long ms){
	int i;
  int j;
  for(i = 0 ; i < ms; i++)
		for (j = 0; j < 2000; j++) 
			__NOP();  
	  
}

