#include "stm32f4xx.h"                  // Device header
#include "Main.h"
#include "Utils.h"
#include "UsartUnit.h"




int main(void) {
	__disable_irq();
	usart_init();
	__enable_irq();
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	while(1) {
	
	}
}


void USART2_IRQHandler(void) {
	NVIC_ClearPendingIRQ(USART2_IRQn);
	
	if(READ_BIT(USART2->SR, USART_SR_RXNE)) { // read enable
		uint16_t data = usart_read();
		terminal_data_handler(data);
	}


}

void terminal_data_handler(uint16_t data) {
	switch (data) {
		case USART_STAR:
			usart_write(data);
			integral_index = 0;
		break;
		case USART_SIGN_NUMBER:
			
			if(integral_index == 3) {
				usart_write(USART_STAR); // clear 7segments
				uint16_t integral = calculator_integral();
				// send integral
				uint16_t digitLeft = integral / 10;
				uint16_t digitRight = integral % 10;
				usart_write(digitLeft);
				usart_write(digitRight);

				integral_index = 0; // reset index
			}
			
		break;
		default: // numbers
			usart_write(data); // echo
			integral_data[integral_index] = data;
			integral_index++;
			
	}
}

uint16_t calculator_integral(void){
	uint16_t a = integral_data[0];
	uint16_t b = integral_data[1];
	uint16_t c = integral_data[2];
	return (uint16_t) (( 45*a + 28*b + 18*c )/12.0f);
}
