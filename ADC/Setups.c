#include "stm32f4xx.h"                  // Device header
#include "Setups.h"




void gpio_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // enable clock GPIO A
	GPIOA->MODER |= GPIO_MODER_MODE0; // set PA0 to analog
	
	// in_buffer, buffer_buffer, buffer_out, end output
	GPIOA->MODER |= GPIO_MODER_MODE1_0 | GPIO_MODER_MODE2_0 | GPIO_MODER_MODE3_0 | GPIO_MODER_MODE4_0;
	
	
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // enable clock GPIO C
	GPIOC->MODER |= 0x55555555; // set all pins as output for buffer_out 
}
	
	
void tim2_init(void){
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	TIM2->PSC = 1600 - 1;
	TIM2->ARR = 100 -1;
	TIM2->CNT = 0;
	
	
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_EnableIRQ(TIM2_IRQn);
}

	
void adc_init(void){

	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // enable clock for adc1
	//ADC1->CR2 |= ADC_CR2_SWSTART; // software start ; do it in timer
	ADC1->CR1 |= ADC_CR1_RES_1; // 8bit resolution
	ADC1->SQR3 &= ~ADC_SQR3_SQ1; // read analog from channel 0
	ADC1->SQR1 &= ~ADC_SQR1_L; // one conversion
	ADC1->CR2 |= ADC_CR2_EOCS;// EOC bit is set after each regular conversion
	
	ADC1->CR2 |= ADC_CR2_ADON;
	
	ADC1->CR1 |= ADC_CR1_EOCIE;
	NVIC_EnableIRQ(ADC_IRQn);
	
	
}

void dma2_init(void){ // use stream0
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	disable_dma2_interrupt_flags();
	NVIC_EnableIRQ(DMA2_Stream0_IRQn);
	NVIC_EnableIRQ(DMA2_Stream5_IRQn);
}

void disable_dma2_interrupt_flags(void){
	// clear all interrupt flags
	DMA2->LIFCR |= DMA_LIFCR_CFEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTCIF0;
}

void dma2_Stream0_inToBuffer(unsigned int source, unsigned int destination, unsigned int length){
	DMA2_Stream0->CR &= ~DMA_SxCR_EN; // disable stream 0
	while(DMA2_Stream0->CR & DMA_SxCR_EN); // wait for stream 0 to disable
	disable_dma2_interrupt_flags();
	DMA2_Stream0->PAR = source; // peripheral address register; address of where data is to be read
	DMA2_Stream0->M0AR = destination; // memory 0 address register;  address of where data is to be written
	DMA2_Stream0->NDTR = length; // length of data to be trasfered
	DMA2_Stream0->CR = 0;
	DMA2_Stream0->CR &= ~DMA_SxCR_CHSEL; // CHSEL = 0 because ADC1 is set on channel 0 for stream 0
	DMA2_Stream0->CR |= DMA_SxCR_PSIZE_0; // Peripheral size = half-word (16bit)
	DMA2_Stream0->CR |= DMA_SxCR_MSIZE_0; // Memory size = half-word (16bit)
	DMA2_Stream0->CR |= DMA_SxCR_MINC; // increament memory address after each dma transfer
	DMA2_Stream0->CR &= ~DMA_SxCR_DIR; //nothing set means Peripheral to memory
	DMA2_Stream0->CR |= DMA_SxCR_TCIE | DMA_SxCR_DMEIE | DMA_SxCR_TEIE; // intterupt after all data transfered
	DMA2_Stream0->FCR = 0; // 0 for dmd means direct mode enable
	DMA2_Stream0->CR |= DMA_SxCR_PL; // priority very high

}

void onInToBufferTransferFinish() {
	//if (DMA2->LISR & 0x000C) /* if an error occurred */
	//		while(1); /* substitute this by error handling */
	DMA2_Stream0->CR &= ~DMA_SxCR_EN; /* disable DMA2 Stream 0 */
	while(DMA2_Stream0->CR & DMA_SxCR_EN);
	disable_dma2_interrupt_flags(); /* clear DMA2 interrupt flags */
	ADC1->CR2 &= ~ADC_CR2_DMA; /* disable ADC conversion complete DMA */
	TIM2->CR1 &= ~TIM_CR1_CEN; /* disable timer2 */
}

void dma2_Stream5_bufferToBuffer(unsigned int sourceMemory, unsigned int destinationMemory, unsigned int length) {
	DMA2_Stream5->CR &= ~DMA_SxCR_EN; // disable stream 0
	while(DMA2_Stream5->CR & DMA_SxCR_EN); // wait for stream 0 to disable
	disable_dma2_interrupt_flags();
	DMA2_Stream5->PAR = sourceMemory; // source memory address register; address of where data is to be read
	DMA2_Stream5->M0AR = destinationMemory; // destination memory address register;  address of where data is to be written
	DMA2_Stream5->NDTR = length; // length of data to be trasfered
	DMA2_Stream5->CR = 0;
	DMA2_Stream5->CR |=  0x6 << DMA_SxCR_CHSEL_Pos; // CHSEL = 6 because TIM1_UPDATE is set on channel 6 for stream 5
	DMA2_Stream5->CR |= DMA_SxCR_PSIZE_0; // Peripheral size = half-word (16bit)
	DMA2_Stream5->CR |= DMA_SxCR_MSIZE_0; // Memory size = half-word (16bit)
	DMA2_Stream5->CR |= DMA_SxCR_MINC; // increament memory address after each dma transfer
	DMA2_Stream5->CR |= DMA_SxCR_DIR; // Memory to Memory
	DMA2_Stream5->CR |= DMA_SxCR_TCIE | DMA_SxCR_DMEIE | DMA_SxCR_TEIE; // intterupt after all data transfered
	DMA2_Stream5->FCR = 0; // 0 for dmd means direct mode enable
	//DMA2_Stream5->CR |= DMA_SxCR_PL; // priority very high; in this case no priority setted
	
}

void dma2_Stream5_bufferToOut(unsigned int memory, unsigned int peripheral, unsigned int length){
	DMA2_Stream5->CR &= ~DMA_SxCR_EN; // disable stream 0
	while(DMA2_Stream5->CR & DMA_SxCR_EN); // wait for stream 0 to disable
	disable_dma2_interrupt_flags();
	DMA2_Stream5->PAR = peripheral; // peripheral address register; address of where data is to be written
	DMA2_Stream5->M0AR = memory; // memory 0 address register;  address of where data is to be read
	DMA2_Stream5->NDTR = length; // length of data to be trasfered
	DMA2_Stream5->CR = 0;
	DMA2_Stream5->CR |=  0x6 << DMA_SxCR_CHSEL_Pos; // CHSEL = 6 because TIM1_UP is set on channel 6 for stream 5
	DMA2_Stream5->CR |= DMA_SxCR_PSIZE_0; // Peripheral size = half-word (16bit)
	DMA2_Stream5->CR |= DMA_SxCR_MSIZE_0; // Memory size = half-word (16bit)
	DMA2_Stream5->CR |= DMA_SxCR_MINC; // increament memory address after each dma transfer
	DMA2_Stream5->CR |= DMA_SxCR_DIR_0; // Memory to peripheral
	DMA2_Stream5->CR |= DMA_SxCR_TCIE | DMA_SxCR_DMEIE | DMA_SxCR_TEIE; // intterupt after all data transfered
	DMA2_Stream5->FCR = 0; // 0 for dmd means direct mode enable
	//DMA2_Stream5->CR |= DMA_SxCR_PL; // priority very high; in this case no priority setted
	
	
}

void enable_stream0_inToBuffer(void) {
	DMA2_Stream0->CR |= DMA_SxCR_EN; // enable dma2 stream 0
	
	// adc1 dma enable is in adc_init();
	ADC1->CR2 |= ADC_CR2_DMA; // enable dma after EOC for adc1
	TIM2->CR1 |= TIM_CR1_CEN;
}

void enable_stream5(void){
	DMA2_Stream5->CR |= DMA_SxCR_EN; // enable dma2 stream 5
	TIM1->CR1 |= TIM_CR1_CEN;
}



void tim1up_init_bufferToOut(void) {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->CR1 &= ~TIM_CR1_CEN; // disable if enable
	TIM1->PSC = 1600 - 1;
	TIM1->ARR = 100 -1;
	TIM1->CNT = 0;
	
	TIM1->DIER = 0;
	TIM1->DIER |= TIM_DIER_UDE; // enable dma request on update tim 1
	
	TIM1->DIER |= TIM_DIER_UIE; // enable interrupt
	NVIC_EnableIRQ(TIM2_IRQn);
	
	
}

void tim1up_init_bufferToBuffer(void) {
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	TIM1->CR1 &= ~TIM_CR1_CEN; // disable if enable
	TIM1->PSC = 1600 - 1;
	TIM1->ARR = 10 -1;
	TIM1->CNT = 0;
	
	TIM1->DIER = 0;
	TIM1->DIER |= TIM_DIER_UDE; // enable dma request on update tim 1
	
	TIM1->DIER |= TIM_DIER_UIE; // enable interrupt
	NVIC_EnableIRQ(TIM2_IRQn);
	
	
}

void onEnd(void) {
	DMA2_Stream5->CR &= ~DMA_SxCR_EN;
	disable_dma2_interrupt_flags();
	TIM1->CR1 &= ~TIM_CR1_CEN;
}


