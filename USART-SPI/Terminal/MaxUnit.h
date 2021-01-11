#include "stm32f4xx.h"                  // Device header
#include "stdint.h"



#define MAX_NO_OP 0x0
#define MAX_DIGIT0 0x1
#define MAX_DIGIT1 0x2
#define MAX_DIGIT2 0x3
#define MAX_DIGIT3 0x4
#define MAX_DECODING_MODE 0x9
#define MAX_INTENSITY_LIGHT 0xA
#define MAX_TURN_ON 0xC
#define MAX_SCAN_LIMIT 0xB
#define MAX_TEST_DISPLAY 0xF

#define MAX_E 10
#define MAX_R 11


static uint8_t max_data[4];

void max_spi_init(void);

void max_write(uint8_t command, uint8_t data);

void max_init(void);

void max_reset(void);

void max_err(void);

void max_putRight(uint8_t data);

void max_loadData(void);

uint8_t max_decode(uint8_t data);
