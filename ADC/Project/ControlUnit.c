#include "stm32f4xx.h"                  
#include "ControlUnit.h"



void resetLED(int pos){
	CONTROL_GPIO->ODR &= ~(1 << pos);
}

void setLED(int pos){
	CONTROL_GPIO->ODR |= (1 << pos);
}

void toggleLED(int pos){
	CONTROL_GPIO->ODR ^= (1 << pos);
}

	
