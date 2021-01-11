#include "stdint.h"

volatile uint16_t integral_index = 0;
volatile uint16_t integral_data[3];

void USART2_IRQHandler(void);

void terminal_data_handler(uint16_t data);

uint16_t calculator_integral(void);
