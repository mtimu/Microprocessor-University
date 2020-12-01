#include "stm32f4xx.h"                  // Device header
#include "stdio.h"
#include "Main.h"
#include "Setups.h"
#include "ControlUnit.h"

#define DATA_SIZE 200
#define STATE_BUFFER_BUFFER 0
#define STATE_BUFFER_OUT 1

volatile static int state = STATE_BUFFER_BUFFER;
volatile static int counter = DATA_SIZE - 1;
volatile static int bufferToBufferMediator;
uint16_t data[DATA_SIZE];
uint16_t processed_data[DATA_SIZE];

	


int main(void) {
	__disable_irq();
	gpio_init();
	dma2_init();
	tim2_init();
	adc_init();
	__enable_irq();
	dma2_Stream0_inToBuffer( (uint32_t) &(ADC1->DR), (uint32_t)data, DATA_SIZE);
	enable_stream0_inToBuffer();
	

	
	while(1) {
	}
	
	
}


// end of in to buffer transfer
void DMA2_Stream0_IRQHandler(void) {
		state = STATE_BUFFER_BUFFER;
		resetLED(IN_BUFFER_LED);
		onInToBufferTransferFinish(); 
	
	// end of sampling from adc
	// enable process and buffer to buffer trasfer
	dma2_Stream5_bufferToBuffer( (uint32_t) &bufferToBufferMediator, (uint32_t) processed_data, DATA_SIZE);
	tim1up_init_bufferToBuffer();
	enable_stream5();
	

}

void ADC_IRQHandler(void){
	toggleLED(IN_BUFFER_LED);
}

void TIM2_IRQHandler(void) {
	TIM2->SR &= ~TIM_SR_UIF_Msk; // reset interrupt request
	TIM2->CNT = 0; // clear counter
	ADC1->CR2 |= ADC_CR2_SWSTART; // start adc conversion
}





void DMA2_Stream5_IRQHandler(void) {
	if(state == STATE_BUFFER_BUFFER){
	// buffer to buffer transfer finished
	
		state = STATE_BUFFER_OUT;
		resetLED(BUFFER_BUFFER_LED);
		// start buffer to output trasfer
		dma2_Stream5_bufferToOut( (uint32_t)processed_data, (uint32_t) &(GPIOA->ODR), DATA_SIZE);
		tim1up_init_bufferToOut();
		enable_stream5();
		
	}else { 
		// buffer to out finished
		// end of all processes
		resetLED(BUFFER_OUT_LED);
		onEnd();
		setLED(END_LED);
	}

}


void TIM1_IRQHandler(void) {
	TIM1->SR &= ~TIM_SR_UIF_Msk; // reset interrupt request
	TIM1->CNT = 0; // clear counter
	
	if(state == STATE_BUFFER_BUFFER) {
		bufferToBufferMediator = data[counter--];
	}else { // STATE_BUFFER_OUT
		
	}
}





