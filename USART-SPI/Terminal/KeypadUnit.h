#include "stm32f4xx.h"                  // Device header
#include "stdint.h"


#ifndef KEYPAD_HEADER

// pins of keypad
#define COL1 0
#define COL2 1
#define COL3 2
#define ROWA 3
#define ROWB 4
#define ROWC 5
#define ROWD 6


#define NO_KEY_PRESSED 15
#define STAR_KEY 9
#define SQUARE_KEY 11

#define KP_GPIO GPIOC

void keypad_init(void);

uint16_t getPressedKeyIndex(void);

uint16_t getPressedKey(void);

#endif
