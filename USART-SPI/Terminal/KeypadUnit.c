#include "stm32f4xx.h"                  // Device header
#include "KeypadUnit.h"
#include "Utils.h"



void keypad_init(void) {
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	
}

uint16_t getPressedKeyIndex() {
	uint16_t col = 0;
	uint16_t row = 0;
	volatile uint32_t input;
	
	
	
	KP_GPIO->MODER |=  out(ROWA) | out(ROWB) | out(ROWC) | out(ROWD);// set rows as output
	KP_GPIO->PUPDR &= pd(COL1) | pd(COL2) | pd(COL3); // set cols PollUp-PollDown as PollDown
	KP_GPIO->ODR = mask(ROWA) | mask(ROWB) | mask(ROWC) | mask(ROWD); // write 1 to rows
	

	input = KP_GPIO->IDR ; // read keypad pins
	
	if(input & mask(0)) col = 0; // PinC0 == 1
	else if(input & mask(1)) col = 1; // PinC1 == 1
	else if(input & mask(2)) col = 2; // PinC2 == 1
  else col = 15; // no button pushed

	

	
	
	KP_GPIO->MODER |= out(COL1) | out(COL2) | out(COL3); // set columns as output
	KP_GPIO->PUPDR &= pd(ROWA) | pd(ROWB) | pd(ROWC) | pd(ROWD); // set rows PollUp-PollDown as PollDown
	KP_GPIO->ODR = mask(COL1) | mask(COL2) | mask(COL3); // write 1 to columns
	
	
	input = KP_GPIO->IDR >> 3; // read keypad pins and shift to right 3times to remove cols input values

	if(input & mask(0)) row = 0; // PinC3 == 1
	else if(input & mask(1)) row = 3; // PinC4 == 1
	else if(input & mask(2)) row = 6; // PinC5 == 1
	else if(input & mask(3)) row = 9; // PinC6 == 1
	else row = 15; // no button pushed

	
	
	if(row == NO_KEY_PRESSED || col == NO_KEY_PRESSED)
	return NO_KEY_PRESSED;
	else
		return row+col;
}


uint16_t getPressedKey(void){
	uint16_t pressed;
	
	while(1)
	{
		pressed = getPressedKeyIndex();
		
		if(pressed == NO_KEY_PRESSED) continue;
		
		pressed = pressed == 10 ? 0 : pressed + 1; // index 10 is number 0.
		break;
	}
	
	
	return pressed;
}
