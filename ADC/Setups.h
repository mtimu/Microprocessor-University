


#define mask(x) (1UL << x)


void gpio_init(void);

void tim2_init(void);


	// configure GPIO
	// enable clock rcc_apb2enr
	// enable adc
	//ADC1->CR2 |= ADC_CR2_ADON;
	// select voltage ref default 0 to 3
	// select trigger source
	// select input channel
	// clock prescaler
	//ADC->CCR |= ADC_CCR_ADCPRE;
	// select other params
void adc_init(void);


void dma2_init(void);

void disable_dma2_interrupt_flags(void);

void dma2_Stream0_inToBuffer(unsigned int source, unsigned int destination, unsigned int length);

void dma2_Stream5_bufferToBuffer(unsigned int sourceMemory, unsigned int destinationMemory, unsigned int length);

void dma2_Stream5_bufferToOut(unsigned int memory, unsigned int peripheral, unsigned int length);

void enable_stream0_inToBuffer(void);

void enable_stream5(void);

void onInToBufferTransferFinish(void);

void onEnd(void);

void tim1up_init_bufferToOut(void);

void tim1up_init_bufferToBuffer(void);
