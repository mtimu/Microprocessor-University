#include "stm32f4xx.h"                  // Device header



#define CONTROL_GPIO GPIOA
#define IN_BUFFER_LED 1
#define BUFFER_BUFFER_LED 2
#define BUFFER_OUT_LED 3
#define END_LED 4


void resetLED(int pos);

void setLED(int pos);

void toggleLED(int pos);

