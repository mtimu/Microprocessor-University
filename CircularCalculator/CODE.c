#include <stm32f4xx.h>


// Operand Tables
// Calculation is base on this Table
// operand			remaing times button pushed % 4
// 		+											0
//		-											1
//		*											2
//		/											3
//

#define mask(x) (1UL << (x))
#define isZero(in,x) (~(in & mask(x)))
#define pd(x) (1 << ((2*x)+1)) // pull down
#define pu(x) (1 << (2*x)) // pull up
#define out(x) (1 << (2*x)) // set as output

// pins of keypad
#define COL1 0
#define COL2 1
#define COL3 2
#define ROWA 3
#define ROWB 4
#define ROWC 5
#define ROWD 6

#define RESET_SEGMENT 0xFF00

// help to read keypad
#define NO_KEY_PRESSED 15
#define STAR_KEY 9
#define SQUARE_KEY 11

// operand
#define OP_PLUS 0
#define OP_MINUS 1
#define OP_MULTI 2
#define OP_DIVIDE 3


volatile int operand = 0;
volatile int x = 0;
volatile int y = 0;
volatile int result = 0;

// an approximation of real delay
void delay(long ms)
{
	int i;
  int j;
  for(i = 0 ; i < ms; i++)
		for (j = 0; j < 2000; j++) 
			__NOP();  
	  
}

// show values on 7segments and after delaying for <ms> reset the 7segments
void show(int left, int right, long ms)
{
	GPIOA->ODR &= RESET_SEGMENT;
	GPIOA->ODR = (left << 4) | right; // left sets on left 7seg and right sets on right 7seg
	delay(ms);
	GPIOA->ODR &= RESET_SEGMENT;
}

// returns index of pushed button. if no button pushed, return 0xF
int getPressedKey()
{
	int col = 0;
	int row = 0;
	int input;
	
	
	
	GPIOC->MODER |=  out(ROWA) | out(ROWB) | out(ROWC) | out(ROWD);// set rows as output
	GPIOC->PUPDR &= pd(COL1) | pd(COL2) | pd(COL3); // set cols PollUp-PollDown as PollDown
	GPIOC->ODR = mask(ROWA) | mask(ROWB) | mask(ROWC) | mask(ROWD); // write 1 to rows
	

	input = GPIOC->IDR ; // read keypad pins
	
	if(input & mask(0)) col = 0; // PinC0 == 1
	else if(input & mask(1)) col = 1; // PinC1 == 1
	else if(input & mask(2)) col = 2; // PinC2 == 1
  else col = 15; // no button pushed

	

	
	
	GPIOC->MODER |= out(COL1) | out(COL2) | out(COL3); // set columns as output
	GPIOC->PUPDR &= pd(ROWA) | pd(ROWB) | pd(ROWC) | pd(ROWD); // set rows PollUp-PollDown as PollDown
	GPIOC->ODR = mask(COL1) | mask(COL2) | mask(COL3); // write 1 to columns
	
	
	input = GPIOC->IDR >> 3; // read keypad pins and shift to right 3times to remove cols input values

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

// wait for user to push a button. buttons star(*) and number sign(#) ignored if pushed
// return actual value of the pushed button
int readKey()
{
	int pressed;
	
	while(1)
	{
		pressed = getPressedKey();
		
		if(pressed == NO_KEY_PRESSED || pressed == SQUARE_KEY || pressed == STAR_KEY) continue;
		
		pressed = pressed == 10 ? 0 : pressed + 1; // index 10 is number 0.
		break;
	}
	
	
	return pressed;
}


// interrupt handler EXTI0 waiting for PinB0 to push
void EXTI0_IRQHandler(void)
{
	EXTI->PR |= mask(0); 
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	if(GPIOB->IDR & mask(0) ) // PinB0 == 1
	{

		// calculation
		if(operand == OP_PLUS) result = x + y;
		else if(operand == OP_MINUS) result = x - y;
		else if(operand == OP_MULTI) result = x * y;
		else if(operand == OP_DIVIDE) result = x / y;
		else result = 0;
		

		show(result / 10, result % 10,2000);
		// change operand base on the table
		operand = (operand + 1) % 4;
		
	}
}



int main() 
{
	// Pins configurations
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; 
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	GPIOA->MODER = 0x5555;
	GPIOB->MODER = 0x0;  

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	// interrupt configurations
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PB;
	EXTI->IMR |= mask(0);
	EXTI->RTSR |= mask(0); 
	__enable_irq();
	NVIC_SetPriority(EXTI0_IRQn,0);
	NVIC_ClearPendingIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI0_IRQn);
	

	// reset
	GPIOA->ODR &= 0xFF00;

	// get numbers x and y
	while(1)
	{
		
		x = readKey();
		show(0,x,1000);
		y = readKey();
		show(0,y,1000);
	}
}



